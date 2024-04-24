#pragma once

#include "../../vulkan/uniform_buffer_type.hpp"

#include "../Base/system.hpp"

#include "../../vulkan/device.hpp"
#include "../../vulkan/frame_info.hpp"
#include "../../vulkan/pipeline.hpp"

// std
#include <memory>
#include <vector>

using namespace vu;

namespace ecs {

// Classe de base pour les systèmes de rendu
class IRenderSystem : public System {
public:
  IRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
  ~IRenderSystem();

  IRenderSystem(const IRenderSystem &) = delete;
  IRenderSystem &operator=(const IRenderSystem &) = delete;

  virtual void render(FrameInfo &frameInfo) = 0;
  virtual void update(FrameInfo &frameInfo, GlobalUbo &ubo);

protected:
  uint32_t mPushConstantRangeSize;
  void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
  virtual void createPipeline(VkRenderPass renderPass);
  void initPipeline(VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

  Device &mVuDevice;
  std::unique_ptr<Pipeline> mVuPipeline;
  VkPipelineLayout mPipelineLayout;
};
} // namespace ecs