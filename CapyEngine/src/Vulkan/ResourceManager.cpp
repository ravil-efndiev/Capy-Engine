#include "ResourceManager.h"

namespace cp {
	VkCommandPool ResourceManager::sCmdPool = VK_NULL_HANDLE;

	void ResourceManager::init(Device& device) {
		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = device.queueFamilies().graphicsFamily.value();
		createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		VkResult result = vkCreateCommandPool(device.vkDevice(), &createInfo, nullptr, &sCmdPool);
		checkVkResult(result, "failed to create resource command pool");
	}

	void ResourceManager::cleanup(Device& device) {
		vkDestroyCommandPool(device.vkDevice(), sCmdPool, nullptr);
	}

	Buffer ResourceManager::createBuffer(
		Device& device,
		size_t size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memProperties
	) {
		Buffer buffer{};

		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.size = size;

		VkResult result = vkCreateBuffer(device.vkDevice(), &createInfo, nullptr, &buffer.buffer);
		checkVkResult(result, "failed to create a buffer");

		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device.vkDevice(), buffer.buffer, &requirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = device.findMemoryType(requirements.memoryTypeBits, memProperties);

		VkResult allocResult = vkAllocateMemory(device.vkDevice(), &allocInfo, nullptr, &buffer.memory);
		checkVkResult(allocResult, "failed to allocate vertex buffer memory");

		vkBindBufferMemory(device.vkDevice(), buffer.buffer, buffer.memory, 0);

		return buffer;
	}

	void ResourceManager::fillBuffer(Device& device, VkDeviceMemory buffMemory, size_t size, const void* data) {
		void* dataTemp;
		vkMapMemory(device.vkDevice(), buffMemory, 0, size, 0, &dataTemp);
		memcpy(dataTemp, data, size);
		vkUnmapMemory(device.vkDevice(), buffMemory);
	}

	void ResourceManager::copyBuffer(Device& device, VkBuffer src, VkBuffer dst, size_t size) {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = sCmdPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device.vkDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkResult beginResult = vkBeginCommandBuffer(commandBuffer, &beginInfo);
		
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;

		vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

		VkResult endResult = vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		
		VkQueue queue = device.graphicsQueue();
		VkResult submitResult = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		checkVkResult({ beginResult, endResult, submitResult }, "failed to copy buffer");

		vkFreeCommandBuffers(device.vkDevice(), sCmdPool, 1, &commandBuffer);
	}
}
