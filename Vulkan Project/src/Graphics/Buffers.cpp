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

		auto [buffer, memory] = ResourceManager::createBuffer(
			mDevice, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		mVertexBuffer = buffer;
		mBufferMemory = memory;

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

		auto [buffer, memory] = ResourceManager::createBuffer(
			mDevice, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		mIndexBuffer = buffer;
		mBufferMemory = memory;

		ResourceManager::copyBuffer(mDevice, stagingBuffer, mIndexBuffer, size);

		vkDestroyBuffer(mDevice.vkDevice(), stagingBuffer, nullptr);
		vkFreeMemory(mDevice.vkDevice(), stagingBufferMemory, nullptr);
	}

	template<class UboT>
	UniformBuffer<UboT>::UniformBuffer(Device& device)
		: mDevice(device) {

		auto [buffer, memory] = ResourceManager::createBuffer(
			mDevice, mSize, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

		mUniformBuffer = buffer;
		mBufferMemory = memory;

		vkMapMemory(mDevice.vkDevice(), mBufferMemory, 0, mSize, 0, &mMappedMemory);
	}

	template<class UboT>
	UniformBuffer<UboT>::~UniformBuffer() {
		vkDestroyBuffer(mDevice.vkDevice(), mUniformBuffer, nullptr);
		vkFreeMemory(mDevice.vkDevice(), mBufferMemory, nullptr);
		CP_DEBUG_LOG("uniform buffer destroyed");
	}

	template<class UboT>
	void UniformBuffer<UboT>::update(const UboT& ubo) {
		memcpy(mMappedMemory, &ubo, mSize);
	}

	template class UniformBuffer<MatrixUBO>;
}
