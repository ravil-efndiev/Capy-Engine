#include "Buffers.h"

namespace cp {
	VertexBuffer::~VertexBuffer() {
		vkDestroyBuffer(mDevice.vkDevice(), mVertexBuffer, nullptr);
		vkFreeMemory(mDevice.vkDevice(), mBufferMemory, nullptr);
		CP_DEBUG_LOG("vertex buffer destroyed");
	}

	void VertexBuffer::create(size_t size, const void* data) {
		auto [stagingBuffer, stagingBufferMemory] = ResourceManager::createBuffer(
			mDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);
		
		ResourceManager::fillBuffer(mDevice, stagingBufferMemory, size, data);

		Buffer vertexBuffer = ResourceManager::createBuffer(
			mDevice, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		mVertexBuffer = vertexBuffer.buffer;
		mBufferMemory = vertexBuffer.memory;

		ResourceManager::copyBuffer(mDevice, stagingBuffer, mVertexBuffer, size);

		vkDestroyBuffer(mDevice.vkDevice(), stagingBuffer, nullptr);
		vkFreeMemory(mDevice.vkDevice(), stagingBufferMemory, nullptr);
	}

	IndexBuffer::IndexBuffer(Device& device, const std::vector<uint16>& indices) 
		: mDevice(device), mIndicesCount(indices.size()) {

		create(sizeof(uint16) * indices.size(), indices.data());
	}

	IndexBuffer::~IndexBuffer() {
		vkDestroyBuffer(mDevice.vkDevice(), mIndexBuffer, nullptr);
		vkFreeMemory(mDevice.vkDevice(), mBufferMemory, nullptr);
		CP_DEBUG_LOG("index buffer destroyed");
	}

	void IndexBuffer::create(size_t size, const uint16* data) {
		auto [stagingBuffer, stagingBufferMemory] = ResourceManager::createBuffer(
			mDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		ResourceManager::fillBuffer(mDevice, stagingBufferMemory, size, data);

		Buffer indexBuffer = ResourceManager::createBuffer(
			mDevice, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		mIndexBuffer = indexBuffer.buffer;
		mBufferMemory = indexBuffer.memory;

		ResourceManager::copyBuffer(mDevice, stagingBuffer, mIndexBuffer, size);

		vkDestroyBuffer(mDevice.vkDevice(), stagingBuffer, nullptr);
		vkFreeMemory(mDevice.vkDevice(), stagingBufferMemory, nullptr);
	}
}
