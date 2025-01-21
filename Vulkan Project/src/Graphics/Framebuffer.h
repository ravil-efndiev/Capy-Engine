#pragma once
#include "RenderPass.h"
#include <Vulkan/Device.h>

namespace cp {
	struct FramebufferSpecification {
		uint width;
		uint height;
		VkImageView attachment;
		RenderPass* renderPass;
	};

	class Framebuffer {
	public:
		Framebuffer(Device& device, const FramebufferSpecification& spec);
		~Framebuffer();

		VkFramebuffer vkHandle() const { return mFramebuffer; }

	private:
		void create();

	private:
		FramebufferSpecification mSpec;
		Device& mDevice;
		VkFramebuffer mFramebuffer = VK_NULL_HANDLE;
	};
}
