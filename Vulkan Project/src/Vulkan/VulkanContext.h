#pragma once
#include <include.h>
#include <Utils.h>

namespace cp {
	struct ApplicationConfiguration {
		std::string_view applicationName = "Capy Engine Application";
		uint applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	};

	class VulkanContext {
	public:
		VulkanContext(const ApplicationConfiguration& appConfig = {});
		~VulkanContext();

		VkInstance instance() const { return mInstance; }

	private:
		void createInstance();
		void createDebugMessenger();
		bool enabledExtensionsAvail() const;

	private:
		const std::array<const char*, 1> mEnabledExtensions = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
		const std::array<const char*, 1> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };

		VkInstance mInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;

		ApplicationConfiguration mAppConfig;
	};
}
