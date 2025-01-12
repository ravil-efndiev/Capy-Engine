#pragma once
#include <Utils.h>

class Window {
public:
	Window(VkInstance instance, int width, int height, const std::string& title);
	~Window();

	GLFWwindow* glfwHandle() { return window_; }
	VkSurfaceKHR surface() const { return surface_; }
	void pollEvents();
	bool shouldClose() const;

private:
	GLFWwindow* window_;
	VkSurfaceKHR surface_ = VK_NULL_HANDLE;
	VkInstance instance_ = VK_NULL_HANDLE;
};
