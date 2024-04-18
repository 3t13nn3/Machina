#pragma once

#include "vulkan/descriptors.hpp"
#include "vulkan/device.hpp"
#include "vulkan/game_object.hpp"
#include "vulkan/renderer.hpp"
#include "vulkan/window.hpp"

// std
#include <memory>
#include <vector>

namespace vu {
class App {
  public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	App();
	~App();

	App(const App &) = delete;
	App &operator=(const App &) = delete;

	void run();

  private:
	void loadGameObjects();

	Window mVuWindow{WIDTH, HEIGHT, "Vulkan Tutorial"};
	Device mVuDevice{mVuWindow};
	Renderer mVuRenderer{mVuWindow, mVuDevice};

	// note: order of declarations matters
	std::unique_ptr<DescriptorPool> mGlobalPool{};
	GameObject::Map mGameObjects;
};
} // namespace vu
