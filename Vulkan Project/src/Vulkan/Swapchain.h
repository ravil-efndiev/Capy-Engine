#pragma once
#include "Device.h"
#include "Window.h"

class Swapchain {
public:
	Swapchain(Device& device, Window& window);
	~Swapchain();

	VkSwapchainKHR vkHandle() const { return swapchain_; }

private:
	void createSwapchain();
	void createImageViews();

	VkSurfaceFormatKHR selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formatsAvail);
	VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& modesAvail);
	VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

private:
	Device& device_;
	Window& window_;
	VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
	std::vector<VkImage> swapchainImages_;
	std::vector<VkImageView> swapchainImageViews_;
	VkSurfaceFormatKHR swapchainFormat_;
	VkExtent2D swapExtent_;
};