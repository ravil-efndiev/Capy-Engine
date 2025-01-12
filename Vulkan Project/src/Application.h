#pragma once
#include "Vulkan/VulkanContext.h"
#include "Vulkan/Window.h"
#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"

class Application {
public:
	Application();
	~Application();
	void run();

private:
	void start();
	void update();

private:
	std::unique_ptr<VulkanContext> context_;
	std::unique_ptr<Window> window_;
	std::unique_ptr<Device> device_;
	std::unique_ptr<Swapchain> swapchain_;
};
