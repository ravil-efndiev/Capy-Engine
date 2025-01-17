#include "Framebuffer.h"

Framebuffer::Framebuffer(Device& device, const FramebufferSpecification& spec) 
	: spec_(spec), device_(device) {
	
	VkFramebufferCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &spec_.attachment;
	createInfo.renderPass = spec_.renderPass->vkHandle();
	createInfo.width = spec_.width;
	createInfo.height = spec_.height;
	createInfo.layers = 1;

	VkResult result = vkCreateFramebuffer(device_.vkDevice(), &createInfo, nullptr, &framebuffer_);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to create framebuffer");
	}
}

Framebuffer::~Framebuffer() {
	vkDestroyFramebuffer(device_.vkDevice(), framebuffer_, nullptr);
	RDEBUG_LOG("framebuffer destroyed");
}
