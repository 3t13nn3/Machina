#pragma once

#include "../../vulkan/model.hpp"
#include "../../vulkan/uniform_buffer_type.hpp"

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../Components/camera.hpp"
#include "../Components/color.hpp"
#include "../Components/point_light.hpp"
#include "../Components/transform.hpp"

#include "render_system.hpp"

// std
#include <memory>
#include <vector>

using namespace vu;

struct PointLightPushConstants {
  glm::vec4 position{};
  glm::vec4 color{};
  float radius;
};

namespace ecs {
class PointLightSystem : public IRenderSystem {
public:
  PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

  void render(FrameInfo &frameInfo) override;
  void update(FrameInfo &frameInfo, GlobalUbo &ubo) override;

protected:
  void createPipelineConfigInfo(PipelineConfigInfo &configInfo);
  void createPipeline(VkRenderPass renderPass) override;
};
} // namespace ecs
