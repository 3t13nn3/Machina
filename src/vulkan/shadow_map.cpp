#include "shadow_map.hpp"
#include <random>
namespace vu {
ShadowMap::ShadowMap(Device &device) : mVuDevice(device) {
  // createRenderPass();
  // createFramebuffers();
  // createSemaphore();
}

ShadowMap::~ShadowMap() {
  vkDestroyImageView(mVuDevice.device(), mImageView, nullptr);
  vkDestroyImage(mVuDevice.device(), mImage, nullptr);
  vkFreeMemory(mVuDevice.device(), mImageMemory, nullptr);
  vkDestroySampler(mVuDevice.device(), mSampler, nullptr);
}

void ShadowMap::createRenderPass() {
  VkAttachmentDescription attachments[2];

  // Depth attachment (shadow map)
  attachments[0].format = VK_FORMAT_D32_SFLOAT;
  attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
  attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  attachments[0].flags = 0;

  // Attachment references from subpasses
  VkAttachmentReference depth_ref;
  depth_ref.attachment = 0;
  depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Subpass 0: shadow map rendering
  VkSubpassDescription subpass[1];
  subpass[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass[0].flags = 0;
  subpass[0].inputAttachmentCount = 0;
  subpass[0].pInputAttachments = NULL;
  subpass[0].colorAttachmentCount = 0;
  subpass[0].pColorAttachments = NULL;
  subpass[0].pResolveAttachments = NULL;
  subpass[0].pDepthStencilAttachment = &depth_ref;
  subpass[0].preserveAttachmentCount = 0;
  subpass[0].pPreserveAttachments = NULL;

  // Create render pass
  VkRenderPassCreateInfo renderPassInfo;
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.pNext = NULL;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = attachments;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = subpass;
  renderPassInfo.dependencyCount = 0;
  renderPassInfo.pDependencies = NULL;
  renderPassInfo.flags = 0;

  if (vkCreateRenderPass(mVuDevice.device(), &renderPassInfo, NULL, &mRenderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass for shadow map!");
  }
}

void ShadowMap::createFramebuffers() {
  VkFramebufferCreateInfo framebufferInfo;
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.pNext = NULL;
  framebufferInfo.renderPass = mRenderPass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = &mImageView;
  framebufferInfo.width = mWidth;
  framebufferInfo.height = mHeight;
  framebufferInfo.layers = 1;
  framebufferInfo.flags = 0;

  if (vkCreateFramebuffer(mVuDevice.device(), &framebufferInfo, NULL, &mFrameBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create framebuffer for shadow map!");
  }
}

void ShadowMap::createSemaphore() {
  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  if (vkCreateSemaphore(mVuDevice.device(), &semaphoreInfo, nullptr, &mSignalSemaphore) !=
      VK_SUCCESS) {
    throw std::runtime_error("Failed to create semaphore for shadow map!");
  }
}

void ShadowMap::submitToQueue(VkCommandBuffer commandBuffer) {
  VkPipelineStageFlags shadow_map_wait_stages = 0;
  VkSubmitInfo submit_info = {};
  submit_info.pNext = NULL;
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.waitSemaphoreCount = 0;
  submit_info.pWaitSemaphores = NULL;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &mSignalSemaphore;
  submit_info.pWaitDstStageMask = 0;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(mVuDevice.mGraphicsQueue, 1, &submit_info, NULL);
}

void ShadowMap::createShadowMapRessources() {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = mWidth;
  imageInfo.extent.height = mHeight;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = VK_FORMAT_D32_SFLOAT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                    VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.flags = 0;

  mVuDevice.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mImage,
                                mImageMemory);

  // Allouez et remplissez une mémoire tampon avec les données souhaitées
  std::vector<float> image_data(mWidth * mHeight);
  for (size_t i = 0; i < image_data.size(); ++i) {
    image_data[i] = 1.f;
  }

  VkDeviceSize imageSize = sizeof(float) * mWidth * mHeight;
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  mVuDevice.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         stagingBuffer, stagingBufferMemory);

  // Copiez les données de l'hôte vers la mémoire tampon
  void *data;
  vkMapMemory(mVuDevice.device(), stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, image_data.data(), (size_t)imageSize);
  vkUnmapMemory(mVuDevice.device(), stagingBufferMemory);

  mVuDevice.transitionImageLayout(mImage, VK_IMAGE_LAYOUT_UNDEFINED,
                                  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
  mVuDevice.copyBufferToImage(stagingBuffer, mImage, mWidth, mHeight, 1);
  mVuDevice.transitionImageLayout(mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);
  // Nettoyez les ressources temporaires
  vkDestroyBuffer(mVuDevice.device(), stagingBuffer, nullptr);
  vkFreeMemory(mVuDevice.device(), stagingBufferMemory, nullptr);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = mImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_D32_SFLOAT;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(mVuDevice.device(), &viewInfo, nullptr, &mImageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shadow map texture image view!");
  }

  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.anisotropyEnable = VK_FALSE;
  samplerInfo.maxAnisotropy = 1.0f;
  samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 1.0f;
  samplerInfo.pNext = nullptr;

  if (vkCreateSampler(mVuDevice.device(), &samplerInfo, nullptr, &mSampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

const VkImageView &ShadowMap::getImageView() const { return mImageView; }

const VkSampler &ShadowMap::getSampler() const { return mSampler; }
} // namespace vu