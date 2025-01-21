#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>

namespace cp {
	class RenderPass {
	public:
		RenderPass(Device& device, Swapchain& swapchain);
		~RenderPass();

		VkRenderPass vkHandle() const { return mPass; }

	private:
		void create();

	private:
		Device& mDevice;
		Swapchain& mSwapchain;
		VkRenderPass mPass = VK_NULL_HANDLE;
	};
}
