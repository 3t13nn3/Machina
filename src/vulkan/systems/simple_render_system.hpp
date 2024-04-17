#pragma once

#include "../camera.hpp"
#include "../device.hpp"
#include "../frame_info.hpp"
#include "../game_object.hpp"
#include "../pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace vu {
class SimpleRenderSystem {
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

	Device &lveDevice;

	std::unique_ptr<Pipeline> lvePipeline;
	VkPipelineLayout pipelineLayout;
};
} // namespace vu
