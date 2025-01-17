#pragma once
#include <Utils.h>
#include <Events/EventHandler.h>

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
	void pollEvents();
	bool shouldClose() const;
	
	void onTest(int width, int height);

private: 
	void registerEvents();

private:
	WindowSpecification spec_{};
	GLFWwindow* window_;
	VkSurfaceKHR surface_ = VK_NULL_HANDLE;
	EventHandler& evtHandler_;
};
