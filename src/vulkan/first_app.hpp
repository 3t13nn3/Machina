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

	Window lveWindow{WIDTH, HEIGHT, "Vulkan Tutorial"};
	Device lveDevice{lveWindow};
	Renderer lveRenderer{lveWindow, lveDevice};

	// note: order of declarations matters
	std::unique_ptr<DescriptorPool> globalPool{};
	GameObject::Map gameObjects;
};
} // namespace vu
