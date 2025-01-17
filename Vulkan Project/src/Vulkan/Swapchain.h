#pragma once
#include "Device.h"
#include "Window.h"

class Swapchain {
public:
	struct Image {
		VkImage image;
		VkImageView view;
	};

	Swapchain(Device& device, Window& window);
	~Swapchain();

	VkSwapchainKHR vkHandle() const { return swapchain_; }
	VkExtent2D extent() const { return swapExtent_; }
	VkSurfaceFormatKHR format() const { return surfaceFormat_; }
	std::vector<Image> images() const;

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
	std::vector<VkImage> images_;
	std::vector<VkImageView> imageViews_;
	VkSurfaceFormatKHR surfaceFormat_;
	VkExtent2D swapExtent_;
};