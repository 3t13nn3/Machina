#include "app.hpp"

#include "ECS/Systems/point_light_system.hpp"
#include "ECS/Systems/simple_render_system.hpp"
#include "vulkan/buffer.hpp"
#include "vulkan/camera.hpp"
#include "vulkan/keyboard_movement_controller.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace vu {

App::App() {
	mGlobalPool = DescriptorPool::Builder(mVuDevice)
					  .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
					  .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
								   SwapChain::MAX_FRAMES_IN_FLIGHT)
					  .build();
	loadGameObjects();
}

App::~App() {}

void App::run() {
	std::vector<std::unique_ptr<Buffer>> uboBuffers(
		SwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < uboBuffers.size(); i++) {
		uboBuffers[i] = std::make_unique<Buffer>(
			mVuDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		uboBuffers[i]->map();
	}

	auto globalSetLayout = DescriptorSetLayout::Builder(mVuDevice)
							   .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
										   VK_SHADER_STAGE_ALL_GRAPHICS)
							   .build();

	std::vector<VkDescriptorSet> globalDescriptorSets(
		SwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < globalDescriptorSets.size(); i++) {
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		DescriptorWriter(*globalSetLayout, *mGlobalPool)
			.writeBuffer(0, &bufferInfo)
			.build(globalDescriptorSets[i]);
	}

	SimpleRenderSystem simpleRenderSystem{
		mVuDevice, mVuRenderer.getSwapChainRenderPass(),
		globalSetLayout->getDescriptorSetLayout()};
	PointLightSystem pointLightSystem{
		mVuDevice, mVuRenderer.getSwapChainRenderPass(),
		globalSetLayout->getDescriptorSetLayout()};
	Camera camera{};

	auto viewerObject = GameObject::createGameObject();
	viewerObject.transform.translation.z = -2.5f;
	KeyboardMovementController cameraController{};

	auto currentTime = std::chrono::high_resolution_clock::now();
	while (!mVuWindow.shouldClose()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime =
			std::chrono::duration<float, std::chrono::seconds::period>(
				newTime - currentTime)
				.count();
		currentTime = newTime;

		cameraController.moveInPlaneXZ(mVuWindow.getGLFWwindow(), frameTime,
									   viewerObject);
		camera.setViewYXZ(viewerObject.transform.translation,
						  viewerObject.transform.rotation);

		float aspect = mVuRenderer.getAspectRatio();
		camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f,
										100.f);

		if (auto commandBuffer = mVuRenderer.beginFrame()) {
			int frameIndex = mVuRenderer.getFrameIndex();
			FrameInfo frameInfo{frameIndex,
								frameTime,
								commandBuffer,
								camera,
								globalDescriptorSets[frameIndex],
								mGameObjects};

			// update
			GlobalUbo ubo{};
			ubo.projection = camera.getProjection();
			ubo.view = camera.getView();
			ubo.inverseView = camera.getInverseView();
			pointLightSystem.update(frameInfo, ubo);
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			mVuRenderer.beginSwapChainRenderPass(commandBuffer);

			// order here matters
			simpleRenderSystem.renderGameObjects(frameInfo);
			pointLightSystem.render(frameInfo);

			mVuRenderer.endSwapChainRenderPass(commandBuffer);
			mVuRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(mVuDevice.device());
}

void App::loadGameObjects() {
	std::shared_ptr<Model> currentModel =
		Model::createModelFromFile(mVuDevice, "models/flat_vase.obj");
	auto flatVase = GameObject::createGameObject();
	flatVase.model = currentModel;
	flatVase.transform.translation = {-.5f, .5f, 0.f};
	flatVase.transform.scale = {3.f, 1.5f, 3.f};
	mGameObjects.emplace(flatVase.getId(), std::move(flatVase));

	currentModel =
		Model::createModelFromFile(mVuDevice, "models/smooth_vase.obj");
	auto smoothVase = GameObject::createGameObject();
	smoothVase.model = currentModel;
	smoothVase.transform.translation = {.5f, .5f, 0.f};
	smoothVase.transform.scale = {3.f, 1.5f, 3.f};
	mGameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

	currentModel = Model::createModelFromFile(mVuDevice, "models/quad.obj");
	auto floor = GameObject::createGameObject();
	floor.model = currentModel;
	floor.transform.translation = {0.f, .5f, 0.f};
	floor.transform.scale = {3.f, 1.f, 3.f};
	mGameObjects.emplace(floor.getId(), std::move(floor));

	std::vector<glm::vec3> lightColors{
		{1.f, .1f, .1f}, {.1f, .1f, 1.f}, {.1f, 1.f, .1f},
		{1.f, 1.f, .1f}, {.1f, 1.f, 1.f}, {1.f, 1.f, 1.f} //
	};

	for (int i = 0; i < lightColors.size(); i++) {
		auto pointLight = GameObject::makePointLight(0.2f);
		pointLight.color = lightColors[i];
		auto rotateLight = glm::rotate(
			glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(),
			{0.f, -1.f, 0.f});
		pointLight.transform.translation =
			glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
		mGameObjects.emplace(pointLight.getId(), std::move(pointLight));
	}
}

} // namespace vu
