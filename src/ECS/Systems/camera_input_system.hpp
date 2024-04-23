#pragma once

#include "../Base/centralizer.hpp"
#include "../Base/system.hpp"

#include "../Components/camera.hpp"
#include "../Components/transform.hpp"

#include "../../vulkan/window.hpp"

using namespace vu;

namespace ecs {
class CameraInputSystem : public System {
  public:
	struct KeyMappings {
		int moveLeft = GLFW_KEY_A;
		int moveRight = GLFW_KEY_D;
		int moveForward = GLFW_KEY_W;
		int moveBackward = GLFW_KEY_S;
		int moveUp = GLFW_KEY_SPACE;
		int moveDown = GLFW_KEY_LEFT_SHIFT;
		int lookLeft = GLFW_KEY_LEFT;
		int lookRight = GLFW_KEY_RIGHT;
		int lookUp = GLFW_KEY_UP;
		int lookDown = GLFW_KEY_DOWN;
	};

	CameraInputSystem(GLFWwindow *window);

	void update(float dt);

	KeyMappings mKeys{};
	float mMoveSpeed{3.f};
	float mLookSpeed{1.5f};

  private:
	GLFWwindow *mWindow;
};
} // namespace ecs
