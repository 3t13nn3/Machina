#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace vu {

class Window {
  public:
	Window(int w, int h, std::string name);
	~Window();

	Window(const Window &) = delete;
	Window &operator=(const Window &) = delete;

	bool shouldClose() { return glfwWindowShouldClose(mWindow); }
	VkExtent2D getExtent() {
		return {static_cast<uint32_t>(mWidth), static_cast<uint32_t>(mHeight)};
	}
	bool wasWindowResized() { return mFramebufferResized; }
	void resetWindowResizedFlag() { mFramebufferResized = false; }
	GLFWwindow *getGLFWwindow() const { return mWindow; }

	void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  private:
	static void framebufferResizeCallback(GLFWwindow *window, int width,
										  int height);
	void initWindow();

	int mWidth;
	int mHeight;
	bool mFramebufferResized = false;

	std::string mWindowName;
	GLFWwindow *mWindow;
};
} // namespace vu
