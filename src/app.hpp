#pragma once

#include "vulkan/descriptors.hpp"
#include "vulkan/device.hpp"
#include "vulkan/renderer.hpp"
#include "vulkan/uniform_buffer.hpp"
#include "vulkan/window.hpp"

// #include "ECS/ECS.hpp"

// std
#include <memory>
#include <vector>

namespace vu {
class App {
public:
  static constexpr int WIDTH = 1600;
  static constexpr int HEIGHT = 1200;

  App();
  ~App();

  App(const App &) = delete;
  App &operator=(const App &) = delete;

  void run();

private:
  void registerComponents();
  void setSignatures();
  void createEntities();

  Window mVuWindow{WIDTH, HEIGHT, "Machina !"};
  Device mVuDevice{mVuWindow};
  Renderer mVuRenderer{mVuWindow, mVuDevice};

  std::unique_ptr<UniformManager> mUniformManager{};
};
} // namespace vu
