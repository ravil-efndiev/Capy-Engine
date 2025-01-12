#include "Swapchain.h"


Swapchain::Swapchain(Device& device, Window& window) : device_(device), window_(window) {
	createSwapchain();
	createImageViews();
}

Swapchain::~Swapchain() {
	for (auto& iv : swapchainImageViews_) {
		vkDestroyImageView(device_.vkDevice(), iv, nullptr);
	}
	vkDestroySwapchainKHR(device_.vkDevice(), swapchain_, nullptr);
	RDEBUG_LOG("swapchain & image views destroyed");
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
	glfwGetFramebufferSize(window_.glfwHandle(), &width, &height);

	VkExtent2D realExtent = { width, height };
	realExtent.width = std::clamp(realExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	realExtent.height = std::clamp(realExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	return realExtent;
}

void Swapchain::createSwapchain() {
	SwapchainSupportDetails details = device_.swapchainDetails();
	swapchainFormat_ = selectSurfaceFormat(details.formats);
	swapExtent_ = selectSwapExtent(details.capabilities);
	VkPresentModeKHR presentMode = selectPresentMode(details.presentModes);

	uint imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
		imageCount = details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = window_.surface();
	createInfo.presentMode = presentMode;
	createInfo.imageExtent = swapExtent_;
	createInfo.imageFormat = swapchainFormat_.format;
	createInfo.imageColorSpace = swapchainFormat_.colorSpace;
	createInfo.minImageCount = imageCount;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices queueFamilies = device_.queueFamilies();
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

	VkResult result = vkCreateSwapchainKHR(device_.vkDevice(), &createInfo, nullptr, &swapchain_);
	RASSERT(result == VK_SUCCESS, "failed to create swapchain");

	uint imgCount;
	vkGetSwapchainImagesKHR(device_.vkDevice(), swapchain_, &imgCount, nullptr);
	swapchainImages_.resize(imgCount);
	vkGetSwapchainImagesKHR(device_.vkDevice(), swapchain_, &imgCount, swapchainImages_.data());
}

void Swapchain::createImageViews() {
	swapchainImageViews_.resize(swapchainImages_.size());
	for (int i = 0; i < swapchainImageViews_.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.image = swapchainImages_[i];
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainFormat_.format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		VkResult result = vkCreateImageView(device_.vkDevice(), &createInfo, nullptr, &swapchainImageViews_[i]);
		RASSERT(result == VK_SUCCESS, "failed to create image view");
	}
}
