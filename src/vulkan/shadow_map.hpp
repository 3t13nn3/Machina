#pragma once

#include <vulkan/vulkan.h>

#include <stdexcept>

#include "device.hpp"

namespace vu {

class ShadowMap {
public:
  ShadowMap(Device &device);
  ~ShadowMap();
  void createShadowMapRessources(uint16_t width = 2048, uint16_t height = 2048);

private:
  Device &mVuDevice;
  VkImage mImage;
  VkImageView mImageView;
  VkDeviceMemory mImageMemory;
  VkSampler mSampler;
};

} // namespace vu
