#include "Device.h"

namespace cp {
	Device::Device(VkInstance instance, VkSurfaceKHR surface) : instance_(instance), surface_(surface) {
		uint deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("no GPUs with Vulkan support found");
		}

		std::vector<VkPhysicalDevice> devicesAvail(deviceCount);
		vkEnumeratePhysicalDevices(instance_, &deviceCount, devicesAvail.data());

		setSuitableDevice(devicesAvail);
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physicalDevice_, &features);

		QueueFamilyIndices indicies = findQueueFamilies(physicalDevice_);

		float queuePriority = 1.f;

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::unordered_set<uint> queueFamilies = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };
		for (uint family : queueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = family;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.queueCreateInfoCount = queueCreateInfos.size();
		createInfo.pEnabledFeatures = &features;

		createInfo.enabledExtensionCount = deviceExtensions_.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions_.data();
		createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_);
		checkVkResult(result, "failed to create Vulkan logical device");

		vkGetDeviceQueue(device_, indicies.graphicsFamily.value(), 0, &graphicsQueue_);
	}

	Device::~Device() {
		vkDestroyDevice(device_, nullptr);
		CP_DEBUG_LOG("device destroyed");
	}

	void Device::wait() {
		vkDeviceWaitIdle(device_);
	}

	void Device::setSuitableDevice(const std::vector<VkPhysicalDevice>& devices) {
		for (VkPhysicalDevice device : devices) {
			if (!deviceValid(device)) continue;
			if (getDeviceType(device) == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				physicalDevice_ = device;
				return;
			}
		}

		for (VkPhysicalDevice device : devices) {
			if (!deviceValid(device)) continue;
			if (getDeviceType(device) == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
				physicalDevice_ = device;
				return;
			}
		}
		throw std::runtime_error("Couldnt find a suitable GPU");
	}

	bool Device::checkDeviceExtSupport(VkPhysicalDevice device) {
		uint deviceExtCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtCount, nullptr);
		std::vector<VkExtensionProperties> extensionsAvail(deviceExtCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtCount, extensionsAvail.data());

		std::unordered_set<std::string> extensionsReq(deviceExtensions_.begin(), deviceExtensions_.end());

		CP_DEBUG_LOG("\DEVICE EXTENSIONS");
		for (const auto& extension : extensionsAvail) {
			CP_DEBUG_LOG("%s", extension.extensionName);
			extensionsReq.erase(extension.extensionName);
		}

		return extensionsReq.empty();
	}

	bool Device::deviceValid(VkPhysicalDevice device) {
		if (!checkDeviceExtSupport(device)) return false;
		if (!findQueueFamilies(device).complete()) return false;

		SwapchainSupportDetails swapchainDetails = querySwapchainSupport(device);
		if (swapchainDetails.formats.empty() || swapchainDetails.presentModes.empty()) return false;
		return true;
	}

	VkPhysicalDeviceType Device::getDeviceType(const VkPhysicalDevice device) {
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(device, &props);

		return props.deviceType;
	}


	QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) const {
		QueueFamilyIndices indices;
		uint queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.graphicsFamily = i;

			VkBool32 presentSupport;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

			if (presentSupport)
				indices.presentFamily = i;

			i++;
		}
		return indices;
	}

	SwapchainSupportDetails Device::querySwapchainSupport(VkPhysicalDevice device) const {
		SwapchainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

		uint formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

		if (formatCount > 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
		}

		uint presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

		if (presentModeCount > 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}
