#include "Framebuffer.h"

namespace cp {
	Framebuffer::Framebuffer(Device& device, const FramebufferSpecification& spec)
		: mSpec(spec), mDevice(device) {

		create();
	}

	Framebuffer::~Framebuffer() {
		vkDestroyFramebuffer(mDevice.vkDevice(), mFramebuffer, nullptr);
		CP_DEBUG_LOG("framebuffer destroyed");
	}

	void Framebuffer::create() {
		VkFramebufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &mSpec.attachment;
		createInfo.renderPass = mSpec.renderPass->vkHandle();
		createInfo.width = mSpec.width;
		createInfo.height = mSpec.height;
		createInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(mDevice.vkDevice(), &createInfo, nullptr, &mFramebuffer);
		checkVkResult(result, "failed to create framebuffer");
	}
}
