#pragma once

#include "device.hpp"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace vu {

class SwapChain {
public:
  static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

  SwapChain(Device &deviceRef, VkExtent2D windowExtent);
  SwapChain(Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);

  ~SwapChain();

  SwapChain(const SwapChain &) = delete;
  SwapChain &operator=(const SwapChain &) = delete;

  VkFramebuffer getFrameBuffer(int index) { return mSwapChainFramebuffers[index]; }
  VkRenderPass getRenderPass() { return mRenderPass; }
  VkImageView getImageView(int index) { return mSwapChainImageViews[index]; }
  size_t imageCount() { return mSwapChainImages.size(); }
  VkFormat getSwapChainImageFormat() { return mSwapChainImageFormat; }
  VkExtent2D getSwapChainExtent() { return mSwapChainExtent; }
  uint32_t width() { return mSwapChainExtent.width; }
  uint32_t height() { return mSwapChainExtent.height; }

  float extentAspectRatio() {
    return static_cast<float>(mSwapChainExtent.width) / static_cast<float>(mSwapChainExtent.height);
  }
  VkFormat findDepthFormat();

  VkResult acquireNextImage(uint32_t *imageIndex);
  VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

  bool compareSwapFormats(const SwapChain &swapChain) const {
    return swapChain.mSwapChainDepthFormat == mSwapChainDepthFormat &&
           swapChain.mSwapChainImageFormat == mSwapChainImageFormat;
  }

private:
  void init();
  void createSwapChain();
  void createImageViews();
  void createDepthResources();
  void createRenderPass();
  void createFramebuffers();
  void createSyncObjects();

  // Helper functions
  VkSurfaceFormatKHR
  chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR
  chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  VkFormat mSwapChainImageFormat;
  VkFormat mSwapChainDepthFormat;
  VkExtent2D mSwapChainExtent;

  std::vector<VkFramebuffer> mSwapChainFramebuffers;
  VkRenderPass mRenderPass;

  std::vector<VkImage> mDepthImages;
  std::vector<VkDeviceMemory> mDepthImageMemorys;
  std::vector<VkImageView> mDepthImageViews;
  std::vector<VkImage> mSwapChainImages;
  std::vector<VkImageView> mSwapChainImageViews;

  Device &mVuDevice;
  VkExtent2D mWindowExtent;

  VkSwapchainKHR mSwapChain;
  std::shared_ptr<SwapChain> mOldSwapChain;

  std::vector<VkSemaphore> mImageAvailableSemaphores;
  std::vector<VkSemaphore> mRenderFinishedSemaphores;
  std::vector<VkFence> mInFlightFences;
  std::vector<VkFence> mImagesInFlight;
  size_t mCurrentFrame = 0;
};

} // namespace vu
