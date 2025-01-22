#pragma once
#include <Vulkan/ResourceManager.h>
#include "Vertex.h"

namespace cp {
	class VertexBuffer {
	public:
		template <class T>
		VertexBuffer(Device& device, const std::vector<T>& verticies) 
			: mDevice(device), mVerticiesCount(verticies.size()) {

			create(sizeof(T) * verticies.size(), reinterpret_cast<const void*>(verticies.data()));
		}
		
		~VertexBuffer();

		VkBuffer vkHandle() const { return mVertexBuffer; }
		size_t vertexCount() const { return mVerticiesCount; }

	private:
		void create(size_t size, const void* data);

	private:
		Device& mDevice;
		VkBuffer mVertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory mBufferMemory = VK_NULL_HANDLE;
		size_t mVerticiesCount = 0;
	};

	class IndexBuffer {
	public:
		IndexBuffer(Device& device, const std::vector<uint16>& indices);
		~IndexBuffer();

		VkBuffer vkHandle() const { return mIndexBuffer; }
		size_t indexCount() const { return mIndicesCount; }

	private:
		void create(size_t size, const uint16* data);

	private:
		Device& mDevice;
		VkBuffer mIndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory mBufferMemory = VK_NULL_HANDLE;
		size_t mIndicesCount = 0;
	};
}
