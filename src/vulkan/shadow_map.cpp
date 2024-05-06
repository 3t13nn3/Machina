#include "shadow_map.hpp"

namespace vu {
ShadowMap::ShadowMap(Device &device) : mVuDevice(device) {}

ShadowMap::~ShadowMap() {
  // vkDestroyImageView(mVuDevice.device(), mImageView, nullptr);
  // vkDestroyImage(mVuDevice.device(), mImage, nullptr);
  // vkFreeMemory(mVuDevice.device(), mImageMemory, nullptr);
  // vkDestroySampler(mVuDevice.device(), mSampler, nullptr);
}

void ShadowMap::createShadowMapRessources(uint16_t width, uint16_t height) {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.flags = 0;

  mVuDevice.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mImage,
                                mImageMemory);

  // Allouez et remplissez une mémoire tampon avec les données souhaitées
  std::vector<uint32_t> image_data(width * height, 0xFF0000FF);
  VkDeviceSize imageSize = sizeof(uint32_t) * width * height;
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  mVuDevice.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);

  // Copiez les données de l'hôte vers la mémoire tampon
  void *data;
  vkMapMemory(mVuDevice.device(), stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, image_data.data(), (size_t)imageSize);
  vkUnmapMemory(mVuDevice.device(), stagingBufferMemory);

  mVuDevice.transitionImageLayout(mImage, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
  mVuDevice.copyBufferToImage(stagingBuffer, mImage, width, height, 1);

  // Transition de layout vers VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL pour une utilisation dans
  // les shaders
  mVuDevice.transitionImageLayout(mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
  // Nettoyez les ressources temporaires
  vkDestroyBuffer(mVuDevice.device(), stagingBuffer, nullptr);
  vkFreeMemory(mVuDevice.device(), stagingBufferMemory, nullptr);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = mImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(mVuDevice.device(), &viewInfo, nullptr, &mImageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shadow map texture image view!");
  }

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.maxAnisotropy = 1.0f;
  samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 1.0f;
  samplerInfo.pNext = nullptr;

  if (vkCreateSampler(mVuDevice.device(), &samplerInfo, nullptr, &mSampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

const VkImageView &ShadowMap::getImageView() const { return mImageView; }

const VkSampler &ShadowMap::getSampler() const { return mSampler; }
} // namespace vu