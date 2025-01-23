#pragma once
#include <Utils.h>

namespace cp {
	class Device {
	public:
		Device(VkInstance instance, VkSurfaceKHR surface);
		~Device();
		VkDevice vkDevice() const { return mDevice; }
		VkPhysicalDevice vkPhysicalDevice() const { return physicalDevice_; }
		SwapchainSupportDetails swapchainDetails() const { return querySwapchainSupport(physicalDevice_); }
		QueueFamilyIndices queueFamilies() const { return findQueueFamilies(physicalDevice_); }
		VkQueue graphicsQueue() const { return mGraphicsQueue; }

		void wait() const;
		
		uint findMemoryType(uint typeFilterBits, VkMemoryPropertyFlags propertyFlags) const;

	private:
		void setSuitableDevice(const std::vector<VkPhysicalDevice>& devices);
		bool deviceValid(VkPhysicalDevice device);
		bool checkDeviceExtSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;
		SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device) const;
		VkPhysicalDeviceType getDeviceType(const VkPhysicalDevice device);

	private:
		VkInstance mInstance = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
		VkDevice mDevice = VK_NULL_HANDLE;
		VkQueue mGraphicsQueue = VK_NULL_HANDLE;
		VkSurfaceKHR mSurface = VK_NULL_HANDLE;

		const std::array<const char*, 1> mDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}