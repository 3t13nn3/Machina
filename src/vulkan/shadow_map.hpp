#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "device.hpp"

namespace vu {

class ShadowMap {
public:
  ShadowMap(Device &device);
  ~ShadowMap();
  void createShadowMapRessources();
  const VkImageView &getImageView() const;
  const VkSampler &getSampler() const;

private:
  void createRenderPass();
  void createFramebuffers();
  void createSemaphore();
  void submitToQueue(VkCommandBuffer commandBuffer);

  Device &mVuDevice;
  VkImage mImage;
  VkImageView mImageView;
  VkDeviceMemory mImageMemory;
  VkSampler mSampler;

  VkRenderPass mRenderPass;
  VkFramebuffer mFrameBuffer;
  VkSemaphore mSignalSemaphore;

  uint16_t mWidth = 2048;
  uint16_t mHeight = 2048;
};

} // namespace vu
