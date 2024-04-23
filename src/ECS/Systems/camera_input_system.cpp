#include "camera_input_system.hpp"

extern std::unique_ptr<ecs::Centralizer> gCentralizer;

namespace ecs {

CameraInputSystem::CameraInputSystem(GLFWwindow *window) : mWindow(window) {}

void CameraInputSystem::update(float dt) {

	for (const Entity &e : mEntities) {
		auto &transform = gCentralizer->getComponent<ecs::Transform>(e);

		glm::vec3 rotate{0};
		if (glfwGetKey(mWindow, mKeys.lookRight) == GLFW_PRESS)
			rotate.y += 1.f;
		if (glfwGetKey(mWindow, mKeys.lookLeft) == GLFW_PRESS)
			rotate.y -= 1.f;
		if (glfwGetKey(mWindow, mKeys.lookUp) == GLFW_PRESS)
			rotate.x += 1.f;
		if (glfwGetKey(mWindow, mKeys.lookDown) == GLFW_PRESS)
			rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			transform.rotation += mLookSpeed * dt * glm::normalize(rotate);
		}

		// limit pitch values between about +/- 85ish degrees
		transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
		transform.rotation.y =
			glm::mod(transform.rotation.y, glm::two_pi<float>());

		float yaw = transform.rotation.y;
		const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
		const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
		const glm::vec3 upDir{0.f, -1.f, 0.f};

		glm::vec3 moveDir{0.f};
		if (glfwGetKey(mWindow, mKeys.moveForward) == GLFW_PRESS)
			moveDir += forwardDir;
		if (glfwGetKey(mWindow, mKeys.moveBackward) == GLFW_PRESS)
			moveDir -= forwardDir;
		if (glfwGetKey(mWindow, mKeys.moveRight) == GLFW_PRESS)
			moveDir += rightDir;
		if (glfwGetKey(mWindow, mKeys.moveLeft) == GLFW_PRESS)
			moveDir -= rightDir;
		if (glfwGetKey(mWindow, mKeys.moveUp) == GLFW_PRESS)
			moveDir += upDir;
		if (glfwGetKey(mWindow, mKeys.moveDown) == GLFW_PRESS)
			moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) >
			std::numeric_limits<float>::epsilon()) {
			transform.position += mMoveSpeed * dt * glm::normalize(moveDir);
		}
	}
}
} // namespace ecs
