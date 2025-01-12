#include "Window.h"

Window::Window(VkInstance instance, int width, int height, const std::string& title) : instance_(instance) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	VkResult result = glfwCreateWindowSurface(instance_, window_, nullptr, &surface_);
	RASSERT(result == VK_SUCCESS, "failed to create window surface");
}

Window::~Window() {
	vkDestroySurfaceKHR(instance_, surface_, nullptr);
	glfwDestroyWindow(window_);
	RDEBUG_LOG("window & surface destroyed");
}

void Window::pollEvents() {
	glfwPollEvents();
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(window_);
}
