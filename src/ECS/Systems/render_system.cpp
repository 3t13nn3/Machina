#include "render_system.hpp"

namespace ecs {

IRenderSystem::IRenderSystem(Device &device, VkRenderPass renderPass,
							 VkDescriptorSetLayout globalSetLayout)
	: mVuDevice{device} {}

IRenderSystem::~IRenderSystem() {
	vkDestroyPipelineLayout(mVuDevice.device(), mPipelineLayout, nullptr);
}

void IRenderSystem::initPipeline(VkRenderPass renderPass,
								 VkDescriptorSetLayout globalSetLayout) {
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

void IRenderSystem::createPipelineLayout(
	VkDescriptorSetLayout globalSetLayout) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags =
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = mPushConstantRangeSize;

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount =
		static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(mVuDevice.device(), &pipelineLayoutInfo, nullptr,
							   &mPipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void IRenderSystem::createPipeline(VkRenderPass renderPass) {}

void IRenderSystem::render(FrameInfo &frameInfo) {}
void IRenderSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {}
} // namespace ecs
