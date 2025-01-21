#include "Window.h"

namespace cp {
	Window::Window(EventHandler& eventHandler, const WindowSpecification& spec)
		: mSpec(spec), mEvtHandler(eventHandler) {

		CP_ASSERT(mSpec.width > 0 && mSpec.height > 0, "cannot initalize a window with 0 width or height");
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		mWindow = glfwCreateWindow(mSpec.width, mSpec.height, mSpec.title.c_str(), nullptr, nullptr);

		VkResult result = glfwCreateWindowSurface(mSpec.instance, mWindow, nullptr, &mSurface);
		checkVkResult(result, "failed to create window surface");
		registerEvents();
	}

	Window::~Window() {
		vkDestroySurfaceKHR(mSpec.instance, mSurface, nullptr);
		glfwDestroyWindow(mWindow);
		CP_DEBUG_LOG("window & surface destroyed");
	}

	void Window::pollEvents() {
		glfwPollEvents();
	}

	bool Window::shouldClose() const {
		return glfwWindowShouldClose(mWindow);
	}

	void Window::wait() {
		glfwWaitEvents();
	}

	void Window::onEvent(const Event* event) {
		mEvtHandler.handle(event);
	}

	void Window::setMinimized(bool flag) {
		mMinimized = flag;
	}

	void Window::registerEvents() {
		glfwSetWindowUserPointer(mWindow, this);

		glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* glfwWin, int width, int height) {
			Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));
			
			ResizeEvent event(width, height);
			window->onEvent(&event);
		});

		glfwSetWindowIconifyCallback(mWindow, [](GLFWwindow* glfwWin, int iconified) {
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
