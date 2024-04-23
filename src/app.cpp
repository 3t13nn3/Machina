#include "app.hpp"

#include "ECS/Systems/camera_input_system.hpp"
#include "ECS/Systems/camera_system.hpp"
#include "ECS/Systems/point_light_system.hpp"
#include "ECS/Systems/simple_render_system.hpp"
#include "vulkan/buffer.hpp"

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

extern std::unique_ptr<ecs::Centralizer> gCentralizer;

namespace vu {

App::App() {
	mGlobalPool = DescriptorPool::Builder(mVuDevice)
					  .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT * 2)
					  .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
								   SwapChain::MAX_FRAMES_IN_FLIGHT * 2)
					  .build();
}

App::~App() {}

void App::registerComponents() {
	gCentralizer->registerComponent<ecs::Model>();
	gCentralizer->registerComponent<ecs::Transform>();
	gCentralizer->registerComponent<ecs::Color>();
	gCentralizer->registerComponent<ecs::PointLight>();
	gCentralizer->registerComponent<ecs::Camera>();
}

void App::setSignatures() {
	ecs::Signature simpleRenderSystemSignature;
	simpleRenderSystemSignature.set(
		gCentralizer->getComponentType<ecs::Model>());
	simpleRenderSystemSignature.set(
		gCentralizer->getComponentType<ecs::Transform>());
	gCentralizer->setSystemSignature<ecs::SimpleRenderSystem>(
		simpleRenderSystemSignature);

	ecs::Signature pointLightSystemSignature;
	pointLightSystemSignature.set(
		gCentralizer->getComponentType<ecs::Transform>());
	pointLightSystemSignature.set(gCentralizer->getComponentType<ecs::Color>());
	pointLightSystemSignature.set(
		gCentralizer->getComponentType<ecs::PointLight>());
	gCentralizer->setSystemSignature<ecs::PointLightSystem>(
		pointLightSystemSignature);

	ecs::Signature cameraSignature;
	cameraSignature.set(gCentralizer->getComponentType<ecs::Camera>());
	cameraSignature.set(gCentralizer->getComponentType<ecs::Transform>());
	gCentralizer->setSystemSignature<ecs::CameraSystem>(cameraSignature);
	gCentralizer->setSystemSignature<ecs::CameraInputSystem>(cameraSignature);
}

void App::createEntities() {
	// Camera
	{
		ecs::Entity e = gCentralizer->createEntity();
		gCentralizer->addComponent(e, ecs::Camera{});
		gCentralizer->addComponent(e,
								   ecs::Transform{{0.f, 0.f, -2.5f}, {}, {}});
	}

	// Object
	{
		ecs::Entity e = gCentralizer->createEntity();
		gCentralizer->addComponent(e, ecs::Transform{{-.5f, .5f, 0.f},
													 {0.f, 0.f, 0.f},
													 {3.f, 1.5f, 3.f}});
		std::shared_ptr<Model> currentModel =
			Model::createModelFromFile(mVuDevice, "models/flat_vase.obj");
		gCentralizer->addComponent(e, ecs::Model{std::move(currentModel)});

		currentModel = Model::createModelFromFile(mVuDevice, "models/cube.obj");
		for (size_t i{0}; i < 30; ++i) {
			for (size_t j{0}; j < 30; ++j) {
				e = gCentralizer->createEntity();
				gCentralizer->addComponent(
					e, ecs::Transform{{(j % 2 ? (i + 0.5f) : (i)) - 15.f,
									   (i % 2 ? j : (j + 0.5f)) - 15.f, 6.f},
									  {0.f, 0.f, 0.f},
									  {0.25f, 0.25f, 0.25f}});
				gCentralizer->addComponent(e, ecs::Model{currentModel});
			}
		}

		e = gCentralizer->createEntity();
		gCentralizer->addComponent(
			e,
			ecs::Transform{{0.f, .5f, 0.f}, {0.f, 0.f, 0.f}, {3.f, 1.f, 3.f}});
		currentModel = Model::createModelFromFile(mVuDevice, "models/quad.obj");
		gCentralizer->addComponent(e, ecs::Model{std::move(currentModel)});
	}

	// Light
	{
		std::vector<glm::vec3> lightColors{
			{1.f, .1f, .1f}, {.1f, .1f, 1.f}, {.1f, 1.f, .1f},
			{1.f, 1.f, .1f}, {.1f, 1.f, 1.f}, {1.f, 1.f, 1.f} //
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto rotateLight = glm::rotate(
				glm::mat4(1.f), (i * glm::two_pi<float>()) / lightColors.size(),
				{0.f, -1.f, 0.f});
			ecs::Entity e = gCentralizer->createEntity();
			gCentralizer->addComponent(
				e, ecs::Transform{glm::vec3(rotateLight *
											glm::vec4(-1.f, -1.f, -1.f, 1.f)),
								  {0.f, 0.f, 0.f},
								  {0.1f, 0.1f, 0.1f}});
			gCentralizer->addComponent(e, ecs::Color{lightColors[i]});
			gCentralizer->addComponent(e, ecs::PointLight{0.2f});
		}
	}
}

void App::run() {

	struct TimeUbo {
		float timeElapsed;
	};

	std::vector<std::unique_ptr<Buffer>> uboBuffers(
		SwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < uboBuffers.size(); i++) {
		uboBuffers[i] = std::make_unique<Buffer>(
			mVuDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		uboBuffers[i]->map();
	}

	std::vector<std::unique_ptr<Buffer>> timeUboBuffers(
		SwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < timeUboBuffers.size(); i++) {
		timeUboBuffers[i] = std::make_unique<Buffer>(
			mVuDevice, sizeof(TimeUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		timeUboBuffers[i]->map();
	}

	auto globalSetLayout = DescriptorSetLayout::Builder(mVuDevice)
							   .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
										   VK_SHADER_STAGE_ALL_GRAPHICS)
							   .addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
										   VK_SHADER_STAGE_ALL_GRAPHICS)
							   .build();

	std::vector<VkDescriptorSet> globalDescriptorSets(
		SwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < globalDescriptorSets.size(); i++) {
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		auto timeBufferInfo =
			timeUboBuffers[i]->descriptorInfo(); // Obtenez les informations sur
												 // le tampon du UBO du temps
		DescriptorWriter(*globalSetLayout, *mGlobalPool)
			.writeBuffer(0, &bufferInfo)
			.writeBuffer(
				1,
				&timeBufferInfo) // Écrivez également le tampon du UBO du temps
			.build(globalDescriptorSets[i]);
	}

	std::shared_ptr<ecs::SimpleRenderSystem> simpleRenderSystem =
		gCentralizer->registerSystem<ecs::SimpleRenderSystem>(
			mVuDevice, mVuRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout());

	std::shared_ptr<ecs::PointLightSystem> pointLightSystem =
		gCentralizer->registerSystem<ecs::PointLightSystem>(
			mVuDevice, mVuRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout());

	std::shared_ptr<ecs::CameraSystem> cameraSystem =
		gCentralizer->registerSystem<ecs::CameraSystem>();

	std::shared_ptr<ecs::CameraInputSystem> cameraInputSystem =
		gCentralizer->registerSystem<ecs::CameraInputSystem>(
			mVuWindow.getGLFWwindow());

	registerComponents();
	setSignatures();
	createEntities();

	auto currentTime = std::chrono::high_resolution_clock::now();
	auto startTime = currentTime;
	while (!mVuWindow.shouldClose()) {
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime =
			std::chrono::duration<float, std::chrono::seconds::period>(
				newTime - currentTime)
				.count();
		currentTime = newTime;

		cameraInputSystem->update(frameTime);

		if (auto commandBuffer = mVuRenderer.beginFrame()) {
			int frameIndex = mVuRenderer.getFrameIndex();
			FrameInfo frameInfo{frameIndex, frameTime, commandBuffer,
								globalDescriptorSets[frameIndex]};

			// update
			GlobalUbo ubo{};
			float aspect = mVuRenderer.getAspectRatio();
			cameraSystem->update(ubo, aspect);
			pointLightSystem->update(frameInfo, ubo);

			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// Initialisez les données du UBO du temps
			TimeUbo timeUboData;
			timeUboData.timeElapsed =
				std::chrono::duration<float, std::chrono::seconds::period>(
					newTime - startTime)
					.count();

			timeUboBuffers[frameIndex]->writeToBuffer(&timeUboData);
			timeUboBuffers[frameIndex]->flush();

			// render
			mVuRenderer.beginSwapChainRenderPass(commandBuffer);

			// order here matters
			simpleRenderSystem->render(frameInfo);
			pointLightSystem->render(frameInfo);

			mVuRenderer.endSwapChainRenderPass(commandBuffer);
			mVuRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(mVuDevice.device());

	// deleting manually the centralizer
	gCentralizer = nullptr;
}

} // namespace vu
