#pragma once

#include "../../vulkan/model.hpp"
#include "../../vulkan/uniform_buffer_type.hpp"

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../Components/camera.hpp"
#include "../Components/color.hpp"
#include "../Components/model.hpp"
#include "../Components/transform.hpp"

#include "render_system.hpp"

// std
#include <map>
#include <memory>
#include <vector>

using namespace vu;

struct SimplePushConstantData {
  glm::mat4 modelMatrix{1.f};
  glm::mat4 normalMatrix{1.f};
  glm::vec3 color{1.f};
  float dist{1.f};
};

namespace ecs {
class SimpleRenderSystem : public IRenderSystem {
public:
  SimpleRenderSystem(Device &device, VkRenderPass renderPass,
                     VkDescriptorSetLayout globalSetLayout);
  void render(FrameInfo &frameInfo) override;
  void update(FrameInfo &frameInfo, GlobalUbo &ubo) override;

protected:
  void createPipelineConfigInfo(PipelineConfigInfo &configInfo);
  void createPipeline(VkRenderPass renderPass) override;
};
} // namespace ecs
