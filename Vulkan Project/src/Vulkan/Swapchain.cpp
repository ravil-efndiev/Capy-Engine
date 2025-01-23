#include "Swapchain.h"

namespace cp {
	Swapchain::Swapchain(Device& device, Window& window) : mDevice(device), mWindow(window) {
		create();
	}

	Swapchain::~Swapchain() {
		destroy();
		CP_DEBUG_LOG("swapchain & image views destroyed");
	}

	VkSurfaceFormatKHR Swapchain::selectSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formatsAvail) {
		for (const auto& format : formatsAvail) {
			if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_R8G8B8A8_SRGB) {
				return format;
			}
		}

		return formatsAvail[0];
	}

	VkPresentModeKHR Swapchain::selectPresentMode(const std::vector<VkPresentModeKHR>& modesAvail) {
		for (auto mode : modesAvail) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Swapchain::selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint>::max()) {
			return capabilities.currentExtent;
		}
		int width, height;
		glfwGetFramebufferSize(mWindow.glfwHandle(), &width, &height);

		VkExtent2D realExtent = { (uint)width, (uint)height };
		realExtent.width = std::clamp(realExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		realExtent.height = std::clamp(realExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return realExtent;
	}

	std::vector<Swapchain::Image> Swapchain::images() const {
		std::vector<Image> images;
		for (size_t i = 0; i < mImages.size(); i++) {
			images.push_back({ mImages[i], mImageViews[i] });
		}
		return images;
	}

	void Swapchain::destroy() {
		for (auto& iv : mImageViews) {
			vkDestroyImageView(mDevice.vkDevice(), iv, nullptr);
		}
		vkDestroySwapchainKHR(mDevice.vkDevice(), mSwapchain, nullptr);
	}

	void Swapchain::create() {
		createSwapchain();
		createImageViews();
	}

	void Swapchain::createSwapchain() {
		SwapchainSupportDetails details = mDevice.swapchainDetails();
		mSurfaceFormat = selectSurfaceFormat(details.formats);
		mSwapExtent = selectSwapExtent(details.capabilities);
		VkPresentModeKHR presentMode = selectPresentMode(details.presentModes);

		uint imageCount = details.capabilities.minImageCount + 1;
		if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
			imageCount = details.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mWindow.surface();
		createInfo.presentMode = presentMode;
		createInfo.imageExtent = mSwapExtent;
		createInfo.imageFormat = mSurfaceFormat.format;
		createInfo.imageColorSpace = mSurfaceFormat.colorSpace;
		createInfo.minImageCount = imageCount;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices queueFamilies = mDevice.queueFamilies();
		uint familiesArr[] = { queueFamilies.graphicsFamily.value(), queueFamilies.presentFamily.value() };
		if (queueFamilies.graphicsFamily != queueFamilies.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = familiesArr;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(mDevice.vkDevice(), &createInfo, nullptr, &mSwapchain);
		checkVkResult(result, "failed to create swapchain");

		uint imgCount;
		vkGetSwapchainImagesKHR(mDevice.vkDevice(), mSwapchain, &imgCount, nullptr);
		mImages.resize(imgCount);
		vkGetSwapchainImagesKHR(mDevice.vkDevice(), mSwapchain, &imgCount, mImages.data());
	}

	void Swapchain::createImageViews() {
		mImageViews.resize(mImages.size());
		for (size_t i = 0; i < mImageViews.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.image = mImages[i];
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = mSurfaceFormat.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			VkResult result = vkCreateImageView(mDevice.vkDevice(), &createInfo, nullptr, &mImageViews[i]);
			checkVkResult(result, "failed to create swapchain image view");
		}
	}
}
