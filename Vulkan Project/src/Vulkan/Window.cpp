#include "Window.h"

namespace cp {
	Window::Window(EventHandler& eventHandler, const WindowSpecification& spec)
		: spec_(spec), evtHandler_(eventHandler) {

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window_ = glfwCreateWindow(spec_.width, spec_.height, spec_.title.c_str(), nullptr, nullptr);
		VkResult result = glfwCreateWindowSurface(spec_.instance, window_, nullptr, &surface_);
		checkVkResult(result, "failed to create window surface");
		registerEvents();
	}

	Window::~Window() {
		vkDestroySurfaceKHR(spec_.instance, surface_, nullptr);
		glfwDestroyWindow(window_);
		CP_DEBUG_LOG("window & surface destroyed");
	}

	void Window::pollEvents() {
		glfwPollEvents();
	}

	bool Window::shouldClose() const {
		return glfwWindowShouldClose(window_);
	}

	void Window::wait() {
		glfwWaitEvents();
	}

	void Window::onEvent(const Event* event) {
		evtHandler_.handle(event);
	}

	void Window::setMinimized(bool flag) {
		minimized_ = flag;
	}

	void Window::registerEvents() {
		glfwSetWindowUserPointer(window_, this);

		glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* glfwWin, int width, int height) {
			Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));
			
			ResizeEvent event(width, height);
			window->onEvent(&event);
		});

		glfwSetWindowIconifyCallback(window_, [](GLFWwindow* glfwWin, int iconified) {
			Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));

			if (iconified) {
				window->setMinimized(true);
			}
			else {
				window->setMinimized(false);
			}
		});
	}
}
