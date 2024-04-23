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

SimpleRenderSystem::SimpleRenderSystem(Device &device, VkRenderPass renderPass,
									   VkDescriptorSetLayout globalSetLayout)
	: IRenderSystem(device, renderPass, globalSetLayout) {
	mPushConstantRangeSize = sizeof(SimplePushConstantData);
	initPipeline(renderPass, globalSetLayout);
}

void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
	assert(mPipelineLayout != nullptr &&
		   "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	// Pipeline::enableAlphaBlending(pipelineConfig);
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = mPipelineLayout;
	mVuPipeline = std::make_unique<Pipeline>(
		mVuDevice, "shaders/simple_shader.vert.spv",
		"shaders/simple_shader.frag.spv", pipelineConfig);
}

void SimpleRenderSystem::render(FrameInfo &frameInfo) {
	mVuPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer,
							VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0,
							1, &frameInfo.globalDescriptorSet, 0, nullptr);

	for (const Entity &e : mEntities) {

		auto &transform = gCentralizer->getComponent<ecs::Transform>(e);
		auto &model = gCentralizer->getComponent<ecs::Model>(e);

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

void SimpleRenderSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {}

} // namespace ecs
