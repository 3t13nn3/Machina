#pragma once

#include <vulkan/vulkan.h>

namespace vu {

struct FrameInfo {
	int frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	VkDescriptorSet globalDescriptorSet;
};
} // namespace vu
