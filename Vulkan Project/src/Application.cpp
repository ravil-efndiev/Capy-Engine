#include "Application.hpp"

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void* userData
) {
	std::cout << "Message severity: " << debugSeverityStr(severity) << "\n";
	std::cout << "Message: " << callbackData->pMessage << "\n";
	return VK_FALSE;
}

void Application::run() {
	start();
	update();
	cleanup();
}

void Application::start() {
	const std::array<const char*, 1> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window_ = glfwCreateWindow(1200, 920, "vk project", nullptr, nullptr);

	std::array<const char*, 1> enabledExtensions = {
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	};

	uint availExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &availExtensionCount, nullptr);
	std::vector<VkExtensionProperties> extensionsAvail(availExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &availExtensionCount, extensionsAvail.data());

	for (const auto& extension : extensionsAvail) {
		RDEBUG_LOG("%s", extension.extensionName);
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
	extensions.reserve(reqExtensionCount + enabledExtensions.size());
	extensions.insert(extensions.end(), requiredExtensions, requiredExtensions + reqExtensionCount);
	extensions.insert(extensions.end(), enabledExtensions.begin(), enabledExtensions.end());

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (RVALIDATION_LAYERS_ENABLED) {
		createInfo.enabledLayerCount = (uint)validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult instanceRes = vkCreateInstance(&createInfo, nullptr, &instance_);
	RASSERT(instanceRes == VK_SUCCESS, "vkCreateInstance failed");

	VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
	messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messengerInfo.messageSeverity = 
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	messengerInfo.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	messengerInfo.pfnUserCallback = debugCallback;

	auto createDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT");
	RASSERT(createDebugMessenger != nullptr, "couldnt get vkCreateDebugUtilsMessengerEXT function");
	VkResult messengerRes = createDebugMessenger(instance_, &messengerInfo, nullptr, &debugMessenger_);
	RASSERT(messengerRes == VK_SUCCESS, "createDebugMessenger aka vkCreateDebugUtilsMessengerEXT failed");
}

void Application::update() {
	while (!glfwWindowShouldClose(window_)) {
		glfwPollEvents();
	}
}

void Application::cleanup() {
	if (RVALIDATION_LAYERS_ENABLED) {
		auto destroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT");
		RASSERT(destroyDebugMessenger != nullptr, "couldnt get vkDestroyDebugUtilsMessengerEXT function");
		destroyDebugMessenger(instance_, debugMessenger_, nullptr);
	}
	vkDestroyInstance(instance_, nullptr);
	glfwDestroyWindow(window_);
	glfwTerminate();
}
