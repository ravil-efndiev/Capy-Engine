#include "Framebuffer.h"

namespace cp {
	Framebuffer::Framebuffer(Device& device, const FramebufferSpecification& spec)
		: spec_(spec), device_(device) {

		create();
	}

	Framebuffer::~Framebuffer() {
		vkDestroyFramebuffer(device_.vkDevice(), framebuffer_, nullptr);
		CP_DEBUG_LOG("framebuffer destroyed");
	}

	void Framebuffer::create() {
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &spec_.attachment;
		createInfo.renderPass = spec_.renderPass->vkHandle();
		createInfo.width = spec_.width;
		createInfo.height = spec_.height;
		createInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(device_.vkDevice(), &createInfo, nullptr, &framebuffer_);
		checkVkResult(result, "failed to create framebuffer");
	}
}
