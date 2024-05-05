#pragma once

#include "buffer.hpp"
#include "descriptors.hpp"
#include "device.hpp"

#include <iostream>
#include <memory>

namespace vu {

class UniformSampler {
public:
  UniformSampler(Device &device, VkShaderStageFlagBits shaderStageType, VkImageView imageView,
                 VkSampler sampler)
      : mVuDevice(device), mSamplers(SwapChain::MAX_FRAMES_IN_FLIGHT),
        mImageView(SwapChain::MAX_FRAMES_IN_FLIGHT), mShaderStageType(shaderStageType) {
    for (int i = 0; i < mSamplers.size(); i++) {
      mSamplers[i] = sampler;
      mImageView[i] = imageView;
    }
  }

  void update(const VkSampler &data, const size_t frameIndex) {}
  const VkShaderStageFlagBits &getShaderStageType() const { return mShaderStageType; }
  const std::vector<VkSampler> &getSamplers() const { return mSamplers; }
  const std::vector<VkImageView> &getImageView() const {
    for (const auto &imageView : mImageView) {
      std::cout << "ImageView: " << imageView << std::endl;
    }
    return mImageView;
  }

private:
  Device &mVuDevice;
  std::vector<VkSampler> mSamplers;
  std::vector<VkImageView> mImageView;
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

    Builder &addUniformSampler(VkShaderStageFlagBits shaderStageType, VkImageView imageView,
                               VkSampler sampler) {
      std::cout << "ICI" << std::endl;
      mUniformSamplers.emplace_back(
          std::make_unique<UniformSampler>(mVuDevice, shaderStageType, imageView, sampler));
      std::cout << "ICI2" << std::endl;
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
                      .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
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
      descriptorSetLayout.addBinding(curr, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                     us->getShaderStageType());
      ++curr;
    }
    mGlobalSetLayout = descriptorSetLayout.build();

    // NEED TO CHANGE THERE
    std::cout << "Size of mGlobalDescriptorSets: " << mGlobalDescriptorSets.size() << std::endl;
    std::cout << "Size of mUniformSamplers: " << mUniformSamplers.size() << std::endl;
    std::cout << "Size of mUniformBuffers: " << mUniformBuffers.size() << std::endl;
    for (int i = 0; i < mGlobalDescriptorSets.size(); i++) {
      DescriptorWriter descriptorWriter(*mGlobalSetLayout, *mGlobalPool);
      // Need to create a temporary vector to handle reference during
      // writeBuffer() until the build function is called
      std::vector<VkDescriptorBufferInfo> bufferInfo(mUniformBuffers.size());
      size_t currBuffer = 0;
      for (const auto &ub : mUniformBuffers) {
        bufferInfo[currBuffer] = ub->getBuffers()[i]->descriptorInfo();
        descriptorWriter.writeBuffer(currBuffer, &(bufferInfo[currBuffer]));
        ++currBuffer;
      }

      std::vector<VkDescriptorImageInfo> imageInfos(mUniformSamplers.size());
      size_t currSampler = 0;
      for (const auto &us : mUniformSamplers) {

        imageInfos[currSampler].sampler = us->getSamplers()[i];
        imageInfos[currSampler].imageView = us->getImageView()[i];
        imageInfos[currSampler].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        // Keep the binding through every uniform type
        descriptorWriter.writeImage(currBuffer + currSampler, &(imageInfos[currSampler]));
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