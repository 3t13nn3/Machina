#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../../vulkan/camera.hpp"
#include "../../vulkan/device.hpp"
#include "../../vulkan/frame_info.hpp"
#include "../../vulkan/game_object.hpp"
#include "../../vulkan/pipeline.hpp"

#include "../Components/color.hpp"
#include "../Components/point_light.hpp"
#include "../Components/transform.hpp"

// std
#include <memory>
#include <vector>

using namespace vu;

namespace ecs {
class PointLightSystem : public ecs::System {
  public:
	PointLightSystem(Device &device, VkRenderPass renderPass,
					 VkDescriptorSetLayout globalSetLayout);
	~PointLightSystem();

	PointLightSystem(const PointLightSystem &) = delete;
	PointLightSystem &operator=(const PointLightSystem &) = delete;

	void update(FrameInfo &frameInfo, GlobalUbo &ubo);
	void render(FrameInfo &frameInfo);

  private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	Device &mVuDevice;

	std::unique_ptr<Pipeline> mVuPipeline;
	VkPipelineLayout mPipelineLayout;
};
} // namespace ecs
