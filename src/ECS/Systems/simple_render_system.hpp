#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../../vulkan/camera.hpp"
#include "../../vulkan/device.hpp"
#include "../../vulkan/frame_info.hpp"
#include "../../vulkan/game_object.hpp"
#include "../../vulkan/pipeline.hpp"

#include "../Components/model.hpp"
#include "../Components/transform.hpp"
// std
#include <memory>
#include <vector>

using namespace vu;

namespace ecs {
class SimpleRenderSystem : public ecs::System {
  public:
	SimpleRenderSystem(Device &device, VkRenderPass renderPass,
					   VkDescriptorSetLayout globalSetLayout);
	~SimpleRenderSystem();

	SimpleRenderSystem(const SimpleRenderSystem &) = delete;
	SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

	void renderGameObjects(FrameInfo &frameInfo);

  private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	Device &mVuDevice;

	std::unique_ptr<Pipeline> mVuPipeline;
	VkPipelineLayout mPipelineLayout;
};
} // namespace ecs
