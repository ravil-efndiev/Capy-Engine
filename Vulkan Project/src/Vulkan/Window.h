#pragma once
#include <Utils.h>
#include <Events/EventHandler.h>

namespace cp {
	struct WindowSpecification {
		VkInstance instance;
		int width;
		int height;
		std::string title;
	};

	class Window {
	public:
		Window(EventHandler& eventHandler, const WindowSpecification& spec);
		~Window();

		GLFWwindow* glfwHandle() { return window_; }
		VkSurfaceKHR surface() const { return surface_; }
		bool minimized() const { return minimized_; }

		void pollEvents();
		bool shouldClose() const;
		void wait();

		void onEvent(const Event* event);
		void setMinimized(bool flag);

	private:
		void registerEvents();

	private:
		WindowSpecification spec_{};
		GLFWwindow* window_;
		VkSurfaceKHR surface_ = VK_NULL_HANDLE;
		EventHandler& evtHandler_;
		bool minimized_ = false;
	};
}
