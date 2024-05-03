#pragma once

#include "descriptors.hpp"
#include "device.hpp"

#include <iostream>
#include <memory>

namespace vu {

class UniformSampler {
public:
  UniformSampler(Device &device, VkShaderStageFlagBits shaderStageType, VkSampler mSampler)
      : mVuDevice(device), mShaderStageType(shaderStageType) {}

  void update(const void *data, const size_t frameIndex) {}

  const VkSampler &getSampler() const { return mSampler; }

  const VkShaderStageFlagBits &getShaderStageType() const { return mShaderStageType; }

private:
  Device &mVuDevice;
  VkSampler mSampler;
  VkShaderStageFlagBits mShaderStageType;
};

class UniformSamplerManager {
public:
  class Builder {
  public:
    Builder(Device &device) : mVuDevice(device) {}

    Builder &addUniformSampler(VkShaderStageFlagBits shaderStageType) {
      mUniformSamplers.emplace_back(std::make_unique<UniformSampler>(mVuDevice, shaderStageType));
      return *this;
    }

    std::unique_ptr<UniformSamplerManager> build() {
      return std::make_unique<UniformSamplerManager>(mVuDevice, std::move(mUniformSamplers));
    }

  private:
    Device &mVuDevice;
    std::vector<std::unique_ptr<UniformSampler>> mUniformSamplers;
  };

  UniformSamplerManager(Device &device,
                        std::vector<std::unique_ptr<UniformSampler>> &&uniformSamplers)
      : mVuDevice(device), mGlobalDescriptorSets(1) {

    mUniformSamplers = std::move(uniformSamplers);

    mGlobalPool = DescriptorPool::Builder(mVuDevice)
                      .setMaxSets(1 * mUniformSamplers.size())
                      .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 * mUniformSamplers.size())
                      .build();

    DescriptorSetLayout::Builder descriptorSetLayout = DescriptorSetLayout::Builder(mVuDevice);
    size_t curr = 0;
    for (const auto &us : mUniformSamplers) {
      descriptorSetLayout.addBinding(curr, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                     us->getShaderStageType());
      ++curr;
    }
    mGlobalSetLayout = descriptorSetLayout.build();

    for (int i = 0; i < mGlobalDescriptorSets.size(); i++) {
      DescriptorWriter descriptorWriter(*mGlobalSetLayout, *mGlobalPool);
      std::vector<VkDescriptorSamplerInfo> samplerInfo(mUniformSamplers.size());

      size_t curr = 0;
      for (const auto &us : mUniformSamplers) {
        samplerInfo[curr] = us->getSamplers()[i]->descriptorInfo();
        descriptorWriter.writeSampler(curr, &(samplerInfo[curr]));
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

  void update(size_t index, const T &data, size_t frameIndex) const {
    if (index < mUniformSamplers.size()) {
      mUniformSamplers[index]->update(data, frameIndex);
    } else {
      throw std::out_of_range("Index out of range");
    }
  }

private:
  Device &mVuDevice;
  std::unique_ptr<DescriptorPool> mGlobalPool{};
  std::unique_ptr<DescriptorSetLayout> mGlobalSetLayout;
  std::vector<VkDescriptorSet> mGlobalDescriptorSets;
  std::vector<std::unique_ptr<UniformSampler>> mUniformSamplers;
};

} // namespace vu