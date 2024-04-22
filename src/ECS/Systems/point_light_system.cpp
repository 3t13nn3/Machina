#include "point_light_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>

extern std::unique_ptr<ecs::Centralizer> gCentralizer;

namespace ecs {

PointLightSystem::PointLightSystem(Device &device, VkRenderPass renderPass,
								   VkDescriptorSetLayout globalSetLayout)
	: IRenderSystem(device, renderPass, globalSetLayout) {
	mPushConstantRangeSize = sizeof(PointLightPushConstants);
	initPipeline(renderPass, globalSetLayout);
}

void PointLightSystem::createPipeline(VkRenderPass renderPass) {
	assert(mPipelineLayout != nullptr &&
		   "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	Pipeline::defaultPipelineConfigInfo(pipelineConfig);
	Pipeline::enableAlphaBlending(pipelineConfig);
	pipelineConfig.attributeDescriptions.clear();
	pipelineConfig.bindingDescriptions.clear();
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = mPipelineLayout;
	mVuPipeline = std::make_unique<Pipeline>(
		mVuDevice, "shaders/point_light.vert.spv",
		"shaders/point_light.frag.spv", pipelineConfig);
}

void PointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo) {
	auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.frameTime,
								   {0.f, -1.f, 0.f});
	for (const Entity &e : mEntities) {
		auto &pointLight = gCentralizer->GetComponent<ecs::PointLight>(e);
		auto &color = gCentralizer->GetComponent<ecs::Color>(e);
		auto &transform = gCentralizer->GetComponent<ecs::Transform>(e);

		assert(e < MAX_LIGHTS && "Point lights exceed maximum specified");

		// update light position
		transform.position =
			glm::vec3(rotateLight * glm::vec4(transform.position, 1.f));

		// copy light to ubo
		ubo.pointLights[e].position = glm::vec4(transform.position, 1.f);
		ubo.pointLights[e].color =
			glm::vec4(color.color, pointLight.lightIntensity);
	}
	ubo.numLights = mEntities.size();
}

void PointLightSystem::render(FrameInfo &frameInfo) {
	// sort lights
	std::map<float, ecs::Entity> sorted;
	for (const Entity &e : mEntities) {

		auto &pointLight = gCentralizer->GetComponent<ecs::PointLight>(e);
		auto &color = gCentralizer->GetComponent<ecs::Color>(e);
		auto &transform = gCentralizer->GetComponent<ecs::Transform>(e);

		// calculate distance
		auto offset = frameInfo.camera.getPosition() - transform.position;
		float disSquared = glm::dot(offset, offset);
		// sorted[disSquared] = obj.getId();
		sorted[disSquared] = e;
	}

	mVuPipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(frameInfo.commandBuffer,
							VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0,
							1, &frameInfo.globalDescriptorSet, 0, nullptr);

	for (const Entity &e : mEntities) {
		// use game obj id to find light object
		auto &transform = gCentralizer->GetComponent<ecs::Transform>(e);
		auto &color = gCentralizer->GetComponent<ecs::Color>(e);
		auto &pointLight = gCentralizer->GetComponent<ecs::PointLight>(e);

		PointLightPushConstants push{};
		push.position = glm::vec4(transform.position, 1.f);
		push.color = glm::vec4(color.color, pointLight.lightIntensity);
		push.radius = transform.scale.x;

		vkCmdPushConstants(frameInfo.commandBuffer, mPipelineLayout,
						   VK_SHADER_STAGE_VERTEX_BIT |
							   VK_SHADER_STAGE_FRAGMENT_BIT,
						   0, sizeof(PointLightPushConstants), &push);
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}

} // namespace ecs
