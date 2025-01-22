#include "VulkanContext.h"

namespace cp {
	VulkanContext::VulkanContext() {
		glfwInit();
		createInstance();
		createDebugMessenger();
	}

	VulkanContext::~VulkanContext() {
		if (gValidationLayersEnabled) {
			auto destroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT");
			CP_ASSERT(destroyDebugMessenger != nullptr, "couldnt get vkDestroyDebugUtilsMessengerEXT function");
			destroyDebugMessenger(mInstance, mDebugMessenger, nullptr);
		}
		CP_DEBUG_LOG("debug messenger destroyed");

		vkDestroyInstance(mInstance, nullptr);
		CP_DEBUG_LOG("instance destroyed");

		glfwTerminate();
		CP_DEBUG_LOG("glfw terminated");
	}

	void VulkanContext::createInstance() {
		if (!enabledExtensionsAvail()) {
			throw std::runtime_error("Required vulkan extensions not available");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "vk project";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "none";

		uint reqExtensionCount = 0;
		const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&reqExtensionCount);
		std::vector<const char*> extensions;
		extensions.reserve(reqExtensionCount + mEnabledExtensions.size());
		extensions.insert(extensions.end(), requiredExtensions, requiredExtensions + reqExtensionCount);
		extensions.insert(extensions.end(), mEnabledExtensions.begin(), mEnabledExtensions.end());

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (gValidationLayersEnabled) {
			createInfo.enabledLayerCount = (uint)mValidationLayers.size();
			createInfo.ppEnabledLayerNames = mValidationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		VkResult instanceRes = vkCreateInstance(&createInfo, nullptr, &mInstance);
		checkVkResult(instanceRes, "couldn't create Vulkan instance");
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData
	) {
		CP_DEBUG_VULKAN(debugSeverityPrefix(severity), callbackData->pMessage);
		return VK_FALSE;
	}

	void VulkanContext::createDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;

		if (gValidationLayersEnabled) {
			auto createDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");
			CP_ASSERT(createDebugMessenger != nullptr, "couldnt get vkCreateDebugUtilsMessengerEXT function");
			VkResult messengerRes = createDebugMessenger(mInstance, &createInfo, nullptr, &mDebugMessenger);
			CP_ASSERT(messengerRes == VK_SUCCESS, "createDebugMessenger aka vkCreateDebugUtilsMessengerEXT failed");
		}
	}

	bool VulkanContext::enabledExtensionsAvail() const {
		uint extCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
		std::vector<VkExtensionProperties> extensionsAvail(extCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extensionsAvail.data());

		std::unordered_set<std::string> extensionsReq(mEnabledExtensions.begin(), mEnabledExtensions.end());

		CP_DEBUG_LOG("\VULKAN EXTENSIONS");
		for (const auto& extension : extensionsAvail) {
			CP_DEBUG_LOG("%s", extension.extensionName);
			extensionsReq.erase(extension.extensionName);
		}

		return extensionsReq.empty();
	}
}
