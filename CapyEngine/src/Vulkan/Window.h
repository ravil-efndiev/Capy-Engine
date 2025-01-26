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

		GLFWwindow* glfwHandle() { return mWindow; }
		VkSurfaceKHR surface() const { return mSurface; }
		bool minimized() const { return mMinimized; }

		void pollEvents();
		bool shouldClose() const;
		void wait();

		void onEvent(const Event* event);
		void setMinimized(bool flag);

	private:
		void registerEvents();

	private:
		WindowSpecification mSpec{};
		GLFWwindow* mWindow;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;
		EventHandler& mEvtHandler;
		bool mMinimized = false;
	};
}
