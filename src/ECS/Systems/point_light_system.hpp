#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../../vulkan/camera.hpp"

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
	PointLightSystem(Device &device, VkRenderPass renderPass,
					 VkDescriptorSetLayout globalSetLayout);

	void render(FrameInfo &frameInfo) override;
	void update(FrameInfo &frameInfo, GlobalUbo &ubo) override;

  protected:
	void createPipeline(VkRenderPass renderPass) override;
};
} // namespace ecs
