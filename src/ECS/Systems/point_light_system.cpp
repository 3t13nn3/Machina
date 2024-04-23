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

	assert(mEntities.size() <= MAX_LIGHTS &&
		   "Point lights exceed maximum specified");

	auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.frameTime,
								   {0.f, -1.f, 0.f});

	size_t lightIndex = 0;
	for (const Entity &e : mEntities) {
		auto &pointLight = gCentralizer->getComponent<ecs::PointLight>(e);
		auto &color = gCentralizer->getComponent<ecs::Color>(e);
		auto &transform = gCentralizer->getComponent<ecs::Transform>(e);

		// update light position
		transform.position =
			glm::vec3(rotateLight * glm::vec4(transform.position, 1.f));

		// copy light to ubo
		ubo.pointLights[lightIndex].position =
			glm::vec4(transform.position, 1.f);
		ubo.pointLights[lightIndex].color =
			glm::vec4(color.color, pointLight.lightIntensity);
		++lightIndex;
	}
	ubo.numLights = mEntities.size();
}

void PointLightSystem::render(FrameInfo &frameInfo) {
	// sort lights
	std::map<float, ecs::Entity> sorted;
	auto &cam = gCentralizer->getComponent<ecs::Camera>(CAMERA_ENTITY);

	for (const Entity &e : mEntities) {

		auto &pointLight = gCentralizer->getComponent<ecs::PointLight>(e);
		auto &color = gCentralizer->getComponent<ecs::Color>(e);
		auto &transform = gCentralizer->getComponent<ecs::Transform>(e);

		// calculate distance
		auto offset = cam.getPosition() - transform.position;
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
		auto &transform = gCentralizer->getComponent<ecs::Transform>(e);
		auto &color = gCentralizer->getComponent<ecs::Color>(e);
		auto &pointLight = gCentralizer->getComponent<ecs::PointLight>(e);

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
