#pragma once

#include "buffer.hpp"
#include "descriptors.hpp"
#include "device.hpp"

#include <iostream>
#include <memory>

namespace vu {

class UniformSampler {
public:
  UniformSampler(Device &device, VkShaderStageFlagBits shaderStageType)
      : mVuDevice(device), mSamplers(SwapChain::MAX_FRAMES_IN_FLIGHT),
        mShaderStageType(shaderStageType) {}
  void update(const VkSampler &data, const size_t frameIndex) {}
  const VkShaderStageFlagBits &getShaderStageType() const { return mShaderStageType; }

private:
  Device &mVuDevice;
  std::vector<VkSampler> mSamplers;
  VkShaderStageFlagBits mShaderStageType;
};

class IUniformBuffer {
public:
  virtual ~IUniformBuffer() = default;
  virtual const std::vector<std::unique_ptr<Buffer>> &getBuffers() const = 0;
  virtual const VkShaderStageFlagBits &getShaderStageType() const = 0;

  template <typename T> void update(const T &data, const size_t frameIndex) {
    updateImpl(&data, frameIndex);
  }

private:
  virtual void updateImpl(const void *data, const size_t frameIndex) = 0;
};

template <typename T> class UniformBuffer : public IUniformBuffer {
public:
  UniformBuffer(Device &device, VkShaderStageFlagBits shaderStageType)
      : mVuDevice(device), mBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT),
        mShaderStageType(shaderStageType) {
    for (int i = 0; i < mBuffers.size(); i++) {
      mBuffers[i] =
          std::make_unique<Buffer>(mVuDevice, sizeof(T), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
      mBuffers[i]->map();
    }
  }

  void updateImpl(const void *data, const size_t frameIndex) override {
    mBuffers[frameIndex]->writeToBuffer(const_cast<void *>(data));
    mBuffers[frameIndex]->flush();
  }

  const std::vector<std::unique_ptr<Buffer>> &getBuffers() const override { return mBuffers; }

  const VkShaderStageFlagBits &getShaderStageType() const override { return mShaderStageType; }

private:
  Device &mVuDevice;
  std::vector<std::unique_ptr<Buffer>> mBuffers;
  VkShaderStageFlagBits mShaderStageType;
};

class UniformManager {
public:
  class Builder {
  public:
    Builder(Device &device) : mVuDevice(device) {}

    template <typename T> Builder &addUniformBuffer(VkShaderStageFlagBits shaderStageType) {
      mUniformBuffers.emplace_back(std::make_unique<UniformBuffer<T>>(mVuDevice, shaderStageType));
      return *this;
    }

    Builder &addUniformSampler(VkShaderStageFlagBits shaderStageType) {
      mUniformSamplers.emplace_back(std::make_unique<UniformSampler>(mVuDevice, shaderStageType));
      return *this;
    }

    std::unique_ptr<UniformManager> build() {
      return std::make_unique<UniformManager>(mVuDevice, std::move(mUniformBuffers),
                                              std::move(mUniformSamplers));
    }

  private:
    Device &mVuDevice;
    std::vector<std::unique_ptr<IUniformBuffer>> mUniformBuffers;
    std::vector<std::unique_ptr<UniformSampler>> mUniformSamplers;
  };

  UniformManager(Device &device, std::vector<std::unique_ptr<IUniformBuffer>> &&uniformBuffers,
                 std::vector<std::unique_ptr<UniformSampler>> &&uniformSamplers)
      : mVuDevice(device), mGlobalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT) {

    mUniformBuffers = std::move(uniformBuffers);
    mUniformSamplers = std::move(uniformSamplers);

    mGlobalPool = DescriptorPool::Builder(mVuDevice)
                      .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT *
                                  (mUniformBuffers.size() + mUniformSamplers.size()))
                      .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   SwapChain::MAX_FRAMES_IN_FLIGHT * mUniformBuffers.size())
                      .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER,
                                   SwapChain::MAX_FRAMES_IN_FLIGHT * mUniformSamplers.size())
                      .build();

    DescriptorSetLayout::Builder descriptorSetLayout = DescriptorSetLayout::Builder(mVuDevice);
    size_t curr = 0;
    for (const auto &ub : mUniformBuffers) {
      descriptorSetLayout.addBinding(curr, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     ub->getShaderStageType());
      ++curr;
    }
    for (const auto &us : mUniformSamplers) {
      descriptorSetLayout.addBinding(curr, VK_DESCRIPTOR_TYPE_SAMPLER, us->getShaderStageType());
      ++curr;
    }
    mGlobalSetLayout = descriptorSetLayout.build();

    // NEED TO CHANGE THERE
    for (int i = 0; i < mGlobalDescriptorSets.size(); i++) {
      DescriptorWriter descriptorWriter(*mGlobalSetLayout, *mGlobalPool);
      // Need to create a temporary vector to handle reference during
      // writeBuffer() until the build function is called
      std::vector<VkDescriptorBufferInfo> bufferInfo(mUniformBuffers.size());

      size_t curr = 0;
      for (const auto &ub : mUniformBuffers) {
        bufferInfo[curr] = ub->getBuffers()[i]->descriptorInfo();
        descriptorWriter.writeBuffer(curr, &(bufferInfo[curr]));
        ++curr;
      }
      descriptorWriter.build(mGlobalDescriptorSets[i]);
    }
  }

  const VkDescriptorSetLayout getDescriptorSetLayout() const {
    return mGlobalSetLayout->getDescriptorSetLayout();
  }

  const std::vector<VkDescriptorSet> &getGlobalDescriptorSets() const {
    return mGlobalDescriptorSets;
  }

  const DescriptorPool &getGlobalPool() const { return *mGlobalPool; }

  template <typename T> void update(size_t index, const T &data, size_t frameIndex) const {
    if (index < mUniformBuffers.size()) {
      mUniformBuffers[index]->update(data, frameIndex);
    } else {
      throw std::out_of_range("Index out of range");
    }
  }

private:
  Device &mVuDevice;

  std::unique_ptr<DescriptorPool> mGlobalPool{};
  std::unique_ptr<DescriptorSetLayout> mGlobalSetLayout;
  std::vector<VkDescriptorSet> mGlobalDescriptorSets;

  std::vector<std::unique_ptr<IUniformBuffer>> mUniformBuffers;
  std::vector<std::unique_ptr<UniformSampler>> mUniformSamplers;
};

} // namespace vu