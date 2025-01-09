#pragma once
#include "include.hpp"
#include "Utils.hpp"

class Application {
public:
	void run();

private:
	void start();
	void update();
	void cleanup();

private:
	GLFWwindow* window_;
	VkInstance instance_ = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;
};
