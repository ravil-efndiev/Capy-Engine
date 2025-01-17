#include "Renderer.h"

Renderer::Renderer(
	Device& device,
	Swapchain& swapchain,
	EventHandler& evtHandler,
	const RendererConfiguration& config
) 
	: device_(device), swapchain_(swapchain), configuration_(config) {

	init();
	createSyncObjects();
}

Renderer::~Renderer() {
	vkDestroyCommandPool(device_.vkDevice(), cmdPool_, nullptr);
	RDEBUG_LOG("command pool destroyed");

	vkDestroySemaphore(device_.vkDevice(), imageAvailSemaphore_, nullptr);
	vkDestroySemaphore(device_.vkDevice(), renderFinishedSemaphore_, nullptr);
	vkDestroyFence(device_.vkDevice(), inFlightFence_, nullptr);
	RDEBUG_LOG("sync objects destroyed");
}

void Renderer::init() {
	pipeline_ = std::make_unique<Pipeline>(device_, swapchain_);

	Shader shader(device_, "assets/shadersbin/vert.spv", "assets/shadersbin/frag.spv");
	pipeline_->setShaderStages(shader);
	pipeline_->create();

	renderPass_ = std::make_unique<RenderPass>(device_, swapchain_);

	std::vector<Swapchain::Image> swapchainImages = swapchain_.images();
	VkExtent2D extent = swapchain_.extent();

	framebuffers_.reserve(swapchainImages.size());
	for (const auto& image : swapchainImages) {
		FramebufferSpecification framebufferSpec{};
		framebufferSpec.width = extent.width;
		framebufferSpec.height = extent.height;
		framebufferSpec.attachment = image.view;
		framebufferSpec.renderPass = renderPass_.get();

		framebuffers_.push_back(std::make_unique<Framebuffer>(device_, framebufferSpec));
	}

	QueueFamilyIndices queueFamilies = device_.queueFamilies();

	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkResult cmdPoolResult = vkCreateCommandPool(device_.vkDevice(), &cmdPoolInfo, nullptr, &cmdPool_);
	if (cmdPoolResult != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool");
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = cmdPool_;
	allocInfo.commandBufferCount = 1;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkResult allocResult = vkAllocateCommandBuffers(device_.vkDevice(), &allocInfo, &cmdBuffer_);
	if (allocResult != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers");
	}
}

void Renderer::createSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkResult result1 = vkCreateSemaphore(device_.vkDevice(), &semaphoreInfo, nullptr, &imageAvailSemaphore_);
	VkResult result2 = vkCreateSemaphore(device_.vkDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore_);
	VkResult result3 = vkCreateFence(device_.vkDevice(), &fenceInfo, nullptr, &inFlightFence_);

	if (result1 != VK_SUCCESS || result2 != VK_SUCCESS || result3 != VK_SUCCESS) {
		throw std::runtime_error("failed to create fence or semaphore");
	}
}

void Renderer::draw() {
	vkWaitForFences(device_.vkDevice(), 1, &inFlightFence_, VK_TRUE, std::numeric_limits<uint64>::max());
	vkResetFences(device_.vkDevice(), 1, &inFlightFence_);

	uint imageIdx = 0;
	vkAcquireNextImageKHR(
		device_.vkDevice(),
		swapchain_.vkHandle(),
		std::numeric_limits<uint64>::max(),
		imageAvailSemaphore_,
		VK_NULL_HANDLE,
		&imageIdx
	);

	vkResetCommandBuffer(cmdBuffer_, 0);
	recordCommandBuffer(imageIdx);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { imageAvailSemaphore_ };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer_;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore_ };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VkResult submitResult = vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, inFlightFence_);
	if (submitResult != VK_SUCCESS) {
		throw std::runtime_error("failed to submit to queue");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { swapchain_.vkHandle() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIdx;

	vkQueuePresentKHR(device_.graphicsQueue(), &presentInfo);
}

void Renderer::finish() {
	vkDeviceWaitIdle(device_.vkDevice());
}

void Renderer::recordCommandBuffer(uint imageIdx) {
	VkCommandBufferBeginInfo bufferBeginInfo{};
	bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	VkResult beginResult = vkBeginCommandBuffer(cmdBuffer_, &bufferBeginInfo);
	if (beginResult != VK_SUCCESS) {
		throw std::runtime_error("failed to begin command buffer");
	}

	VkRenderPassBeginInfo passBeginInfo{};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.renderPass = renderPass_->vkHandle();
	passBeginInfo.framebuffer = framebuffers_[imageIdx]->vkHandle();
	passBeginInfo.renderArea.extent = swapchain_.extent();
	passBeginInfo.renderArea.offset = { 0, 0 };
		
	VkClearValue clearColor = {{{ 0.f, 0.f, 0.f, 1.f }}};
	passBeginInfo.clearValueCount = 1;
	passBeginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(cmdBuffer_, &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmdBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->vkHandle());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)swapchain_.extent().width;
	viewport.height = (float)swapchain_.extent().width;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmdBuffer_, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain_.extent();
	vkCmdSetScissor(cmdBuffer_, 0, 1, &scissor);

	vkCmdDraw(cmdBuffer_, 3, 1, 0, 0);

	vkCmdEndRenderPass(cmdBuffer_);

	VkResult endBufferResult = vkEndCommandBuffer(cmdBuffer_);
	if (endBufferResult != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer");
	}
}
