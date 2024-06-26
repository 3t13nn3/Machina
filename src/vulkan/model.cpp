#include "model.hpp"

#include "utils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <iostream>
#include <unordered_map>

#ifndef ENGINE_DIR
#define ENGINE_DIR "../"
#endif

namespace std {
template <> struct hash<vu::Model::Vertex> {
  size_t operator()(vu::Model::Vertex const &vertex) const {
    size_t seed = 0;
    vu::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
    return seed;
  }
};
} // namespace std

namespace vu {

Model::Model(Device &device, const Model::Builder &builder) : mVuDevice{device} {
  createVertexBuffers(builder.vertices);
  createIndexBuffers(builder.indices);
}

Model::~Model() {}

std::unique_ptr<Model> Model::createModelFromFile(Device &device, const std::string &filepath) {
  Builder builder{};
  builder.loadModel(ENGINE_DIR + filepath);
  return std::make_unique<Model>(device, builder);
}

void Model::createVertexBuffers(const std::vector<Vertex> &vertices) {
  mVertexCount = static_cast<uint32_t>(vertices.size());
  assert(mVertexCount >= 3 && "Vertex count must be at least 3");
  VkDeviceSize bufferSize = sizeof(vertices[0]) * mVertexCount;
  uint32_t vertexSize = sizeof(vertices[0]);

  Buffer stagingBuffer{
      mVuDevice,
      vertexSize,
      mVertexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)vertices.data());

  mVertexBuffer =
      std::make_unique<Buffer>(mVuDevice, vertexSize, mVertexCount,
                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  mVuDevice.copyBuffer(stagingBuffer.getBuffer(), mVertexBuffer->getBuffer(), bufferSize);
}

void Model::createIndexBuffers(const std::vector<uint32_t> &indices) {
  mIndexCount = static_cast<uint32_t>(indices.size());
  mHasIndexBuffer = mIndexCount > 0;

  if (!mHasIndexBuffer) {
    return;
  }

  VkDeviceSize bufferSize = sizeof(indices[0]) * mIndexCount;
  uint32_t indexSize = sizeof(indices[0]);

  Buffer stagingBuffer{
      mVuDevice,
      indexSize,
      mIndexCount,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
  };

  stagingBuffer.map();
  stagingBuffer.writeToBuffer((void *)indices.data());

  mIndexBuffer =
      std::make_unique<Buffer>(mVuDevice, indexSize, mIndexCount,
                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  mVuDevice.copyBuffer(stagingBuffer.getBuffer(), mIndexBuffer->getBuffer(), bufferSize);
}

void Model::draw(VkCommandBuffer commandBuffer) {
  if (mHasIndexBuffer) {
    vkCmdDrawIndexed(commandBuffer, mIndexCount, 1, 0, 0, 0);
  } else {
    vkCmdDraw(commandBuffer, mVertexCount, 1, 0, 0);
  }
}

void Model::bind(VkCommandBuffer commandBuffer) {
  VkBuffer buffers[] = {mVertexBuffer->getBuffer()};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

  if (mHasIndexBuffer) {
    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  }
}

std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescriptions() {
  std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(Vertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions() {
  std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

  attributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
  attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color)});
  attributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
  attributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv)});

  return attributeDescriptions;
}

void Model::Builder::loadModel(const std::string &filepath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filepath.c_str())) {
    throw std::runtime_error(err);
  }

  vertices.clear();
  indices.clear();

  std::unordered_map<Vertex, uint32_t> uniqueVertices{};
  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      if (index.vertex_index >= 0) {
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2],
        };

        vertex.color = glm::vec3{1.f, 1.f, 1.f}; // white by default
      }

      if (index.normal_index >= 0) {
        vertex.normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2],
        };
      }

      if (index.texcoord_index >= 0) {
        vertex.uv = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            attrib.texcoords[2 * index.texcoord_index + 1],
        };
      }

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }
      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

} // namespace vu
