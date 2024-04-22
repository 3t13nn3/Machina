#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../../vulkan/camera.hpp"

#include "../Components/model.hpp"
#include "../Components/transform.hpp"

#include "render_system.hpp"

// std
#include <memory>
#include <vector>

using namespace vu;

struct SimplePushConstantData {
	glm::mat4 modelMatrix{1.f};
	glm::mat4 normalMatrix{1.f};
};

namespace ecs {
class SimpleRenderSystem : public IRenderSystem {
  public:
	SimpleRenderSystem(Device &device, VkRenderPass renderPass,
					   VkDescriptorSetLayout globalSetLayout);
	void render(FrameInfo &frameInfo) override;
	void update(FrameInfo &frameInfo, GlobalUbo &ubo) override;

  protected:
	void createPipeline(VkRenderPass renderPass) override;
};
} // namespace ecs
