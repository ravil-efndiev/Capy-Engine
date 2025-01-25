#pragma once
#include "Device.h"
#include "Window.h"

namespace cp {
	class Swapchain {
	public:
		struct Image {
			VkImage image;
			VkImageView view;
		};

		Swapchain(Device& device, Window& window);
		~Swapchain();

		VkSwapchainKHR vkHandle() const { return mSwapchain; }
		VkExtent2D extent() const { return mSwapExtent; }
		VkSurfaceFormatKHR format() const { return mSurfaceFormat; }
		std::vector<Image> images() const;

		void destroy();
		void create();

	private:
		void createSwapchain();
		void createImageViews();

		VkSurfaceFormatKHR selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formatsAvail);
		VkPresentModeKHR selectPresentMode(const std::vector<VkPresentModeKHR>& modesAvail);
		VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		Device& mDevice;
		Window& mWindow;
		VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
		std::vector<VkImage> mImages;
		std::vector<VkImageView> mImageViews;
		VkSurfaceFormatKHR mSurfaceFormat;
		VkExtent2D mSwapExtent;
	};
}