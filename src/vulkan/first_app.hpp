#pragma once

#include "descriptors.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "renderer.hpp"
#include "window.hpp"

// std
#include <memory>
#include <vector>

namespace vu {
class FirstApp {
  public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	FirstApp();
	~FirstApp();

	FirstApp(const FirstApp &) = delete;
	FirstApp &operator=(const FirstApp &) = delete;

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
