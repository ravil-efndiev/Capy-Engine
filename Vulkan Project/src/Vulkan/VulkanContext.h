#pragma once
#include <include.h>
#include <Utils.h>

namespace cp {
	class VulkanContext {
	public:
		VulkanContext();
		~VulkanContext();

		VkInstance instance() const { return instance_; }

	private:
		void createInstance();
		void createDebugMessenger();
		bool enabledExtensionsAvail() const;

	private:
		const std::array<const char*, 1> enabledExtensions_ = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
		const std::array<const char*, 1> validationLayers_ = { "VK_LAYER_KHRONOS_validation" };

		VkInstance instance_ = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger_ = VK_NULL_HANDLE;
	};
}
