#include "Window.h"

Window::Window(EventHandler& eventHandler, const WindowSpecification& spec) 
	: spec_(spec), evtHandler_(eventHandler) {

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window_ = glfwCreateWindow(spec_.width, spec_.height, spec_.title.c_str(), nullptr, nullptr);
	VkResult result = glfwCreateWindowSurface(spec_.instance, window_, nullptr, &surface_);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
	registerEvents();
}

Window::~Window() {
	vkDestroySurfaceKHR(spec_.instance, surface_, nullptr);
	glfwDestroyWindow(window_);
	RDEBUG_LOG("window & surface destroyed");
}

void Window::pollEvents() {
	glfwPollEvents();
}

bool Window::shouldClose() const {
	return glfwWindowShouldClose(window_);
}

void Window::onTest(int width, int height) {
	evtHandler_.handleTest(width, height);
}

void Window::registerEvents() {
	glfwSetWindowUserPointer(window_, this);
	glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* glfwWin, int width, int height) {
		Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));
		window->onTest(width, height);
	});
}
