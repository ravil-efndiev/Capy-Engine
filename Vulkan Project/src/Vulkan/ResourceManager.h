#pragma once
#include "Device.h"

namespace cp {
	struct Buffer {
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
	};

	class ResourceManager {
	public:
		static void init(Device& device);
		static void cleanup(Device& device);

		static Buffer createBuffer(
			Device& device,
			size_t size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags memProperties
		);

		static void fillBuffer(Device& device, VkDeviceMemory buffMemory, size_t size, const void* data);

		static void copyBuffer(Device& device, VkBuffer src, VkBuffer dst, size_t size);

	private:
		static VkCommandPool sCmdPool;
	};
}
