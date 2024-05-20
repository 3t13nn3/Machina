#include "render_system.hpp"

extern std::unique_ptr<ecs::Centralizer> gCentralizer;

namespace ecs {

IRenderSystem::IRenderSystem(Device &device, VkRenderPass renderPass,
                             VkDescriptorSetLayout globalSetLayout)
    : mVuDevice{device} {}

IRenderSystem::~IRenderSystem() {
  vkDestroyPipelineLayout(mVuDevice.device(), mPipelineLayout, nullptr);
}

void IRenderSystem::initPipeline(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
  createPipelineLayout(globalSetLayout);
  createPipeline(renderPass);
}

void IRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  pushConstantRange.offset = 0;
  pushConstantRange.size = mPushConstantRangeSize;

  std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
  pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
  pipelineLayoutInfo.pushConstantRangeCount = 1;
  pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
  if (vkCreatePipelineLayout(mVuDevice.device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

std::map<float, ecs::Entity> const
IRenderSystem::getSortedEntities(const std::set<Entity> &entities, bool withY) {
  std::map<float, ecs::Entity> sorted;

  // Récupérer la caméra à partir du centralizer
  auto &cam = gCentralizer->getComponent<ecs::Camera>(CAMERA_ENTITY);
  glm::vec3 camPos = cam.getPosition();
  if (!withY)
    camPos.y = 0.0f;

  for (const Entity &e : entities) {
    auto &transform = gCentralizer->getComponent<ecs::Transform>(e);
    auto &color = gCentralizer->getComponent<ecs::Color>(e);
    // glm::vec3 cameraToObject = transform.position - camera.getPosition();
    glm::vec3 elementPos = transform.position;
    if (!withY)
      elementPos.y = 0.0f;
    float distance = 1.f;
    // if (glm::dot(cameraToObject, viewDirection) > 0) {
    if (color.color != glm::vec3{1.f}) {
      distance =
          0.5 * glm::length(camPos - elementPos) * glm::length(camPos - elementPos) / 80.f + 0.1f;
    } else {
      distance = 1.f;
    }
    sorted.emplace(distance + transform.position.y * 0.0001f,
                   e); // transform.position.y * 0.0001f to ensure unicity on map
    //}
  }

  return sorted;
}

void IRenderSystem::createPipeline(VkRenderPass renderPass) {}

void IRenderSystem::render(FrameInfo &frameInfo) {}
void IRenderSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {}
} // namespace ecs
