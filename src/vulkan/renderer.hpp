#pragma once

#include "device.hpp"
#include "swap_chain.hpp"
#include "window.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace vu {
class Renderer {
  public:
	Renderer(Window &window, Device &device);
	~Renderer();

	Renderer(const Renderer &) = delete;
	Renderer &operator=(const Renderer &) = delete;

	VkRenderPass getSwapChainRenderPass() const {
		return mSwapChain->getRenderPass();
	}
	float getAspectRatio() const { return mSwapChain->extentAspectRatio(); }
	bool isFrameInProgress() const { return mIsFrameStarted; }

	VkCommandBuffer getCurrentCommandBuffer() const {
		assert(mIsFrameStarted &&
			   "Cannot get command buffer when frame not in progress");
		return mCommandBuffers[mCurrentFrameIndex];
	}

	int getFrameIndex() const {
		assert(mIsFrameStarted &&
			   "Cannot get frame index when frame not in progress");
		return mCurrentFrameIndex;
	}

	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapChain();

	Window &mVuWindow;
	Device &mVuDevice;
	std::unique_ptr<SwapChain> mSwapChain;
	std::vector<VkCommandBuffer> mCommandBuffers;

	uint32_t mCurrentImageIndex;
	int mCurrentFrameIndex{0};
	bool mIsFrameStarted{false};
};
} // namespace vu
