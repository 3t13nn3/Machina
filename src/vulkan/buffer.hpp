#pragma once

#include "device.hpp"

namespace vu {

class Buffer {
public:
  Buffer(Device &device, VkDeviceSize instanceSize, uint32_t instanceCount,
         VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags,
         VkDeviceSize minOffsetAlignment = 1);
  ~Buffer();

  Buffer(const Buffer &) = delete;
  Buffer &operator=(const Buffer &) = delete;

  VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  void unmap();

  void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
  VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

  void writeToIndex(void *data, int index);
  VkResult flushIndex(int index);
  VkDescriptorBufferInfo descriptorInfoForIndex(int index);
  VkResult invalidateIndex(int index);

  VkBuffer getBuffer() const { return mBuffer; }
  void *getMappedMemory() const { return mMapped; }
  uint32_t getInstanceCount() const { return mInstanceCount; }
  VkDeviceSize getInstanceSize() const { return mInstanceSize; }
  VkDeviceSize getAlignmentSize() const { return mInstanceSize; }
  VkBufferUsageFlags getUsageFlags() const { return mUsageFlags; }
  VkMemoryPropertyFlags getMemoryPropertyFlags() const { return mMemoryPropertyFlags; }
  VkDeviceSize getBufferSize() const { return mBufferSize; }

private:
  static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

  Device &mVuDevice;
  void *mMapped = nullptr;
  VkBuffer mBuffer = VK_NULL_HANDLE;
  VkDeviceMemory mMemory = VK_NULL_HANDLE;

  VkDeviceSize mBufferSize;
  uint32_t mInstanceCount;
  VkDeviceSize mInstanceSize;
  VkDeviceSize mAlignmentSize;
  VkBufferUsageFlags mUsageFlags;
  VkMemoryPropertyFlags mMemoryPropertyFlags;
};

} // namespace vu
