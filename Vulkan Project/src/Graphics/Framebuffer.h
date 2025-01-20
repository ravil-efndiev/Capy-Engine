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

		VkFramebuffer vkHandle() const { return framebuffer_; }

	private:
		void create();

	private:
		FramebufferSpecification spec_;
		Device& device_;
		VkFramebuffer framebuffer_ = VK_NULL_HANDLE;
	};
}
