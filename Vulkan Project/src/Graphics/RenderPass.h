#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>

namespace cp {
	class RenderPass {
	public:
		RenderPass(Device& device, Swapchain& swapchain);
		~RenderPass();

		VkRenderPass vkHandle() const { return pass_; }

	private:
		void create();

	private:
		Device& device_;
		Swapchain& swapchain_;
		VkRenderPass pass_ = VK_NULL_HANDLE;
	};
}
