#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

extern std::unique_ptr<ecs::Centralizer> gCentralizer;

namespace ecs {

struct SimplePushConstantData {
	glm::mat4 modelMatrix{1.f};
	glm::mat4 normalMatrix{1.f};
};

SimpleRenderSystem::SimpleRenderSystem(Device &device, VkRenderPass renderPass,
									   VkDescriptorSetLayout globalSetLayout)
	: mVuDevice{device} {
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

SimpleRenderSystem::~SimpleRenderSystem() {
	vkDestroyPipelineLayout(mVuDevice.device(), mPipelineLayout, nullptr);
}

void SimpleRenderSystem::createPipelineLayout(
	VkDescriptorSetLayout globalSetLayout) {
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags =
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

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

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
	assert(mPipelineLayout != nullptr &&
		   "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = mPipelineLayout;
	mVuPipeline = std::make_unique<Pipeline>(
		mVuDevice, "shaders/simple_shader.vert.spv",
		"shaders/simple_shader.frag.spv", pipelineConfig);
}

void SimpleRenderSystem::renderGameObjects(FrameInfo &frameInfo) {
	mVuPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer,
							VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0,
							1, &frameInfo.globalDescriptorSet, 0, nullptr);

	for (const Entity &e : mEntities) {

		auto &transform = gCentralizer->GetComponent<ecs::Transform>(e);
		auto &model = gCentralizer->GetComponent<ecs::Model>(e);

		if (model.model == nullptr)
			continue;
		SimplePushConstantData push{};
		push.modelMatrix = transform.mat4();
		push.normalMatrix = transform.normalMatrix();

		vkCmdPushConstants(frameInfo.commandBuffer, mPipelineLayout,
						   VK_SHADER_STAGE_VERTEX_BIT |
							   VK_SHADER_STAGE_FRAGMENT_BIT,
						   0, sizeof(SimplePushConstantData), &push);
		model.model->bind(frameInfo.commandBuffer);
		model.model->draw(frameInfo.commandBuffer);
	}
}

} // namespace ecs
