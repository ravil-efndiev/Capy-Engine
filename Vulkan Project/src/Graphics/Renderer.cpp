#include "Renderer.h"

namespace cp {
	Renderer::Renderer(
		Device& device,
		Swapchain& swapchain,
		EventHandler& evtHandler,
		const RendererConfiguration& config
	)
		: device_(device), swapchain_(swapchain), configuration_(config) {

		viewportWidth_ = swapchain_.extent().width;
		viewportHeight_ = swapchain_.extent().height;

		init();
		createSyncObjects();

		evtHandler.subscribeToResizeEvt([&](int width, int height) {
			setViewportSize(width, height);
		});
	}

	Renderer::~Renderer() {
		vkDestroyCommandPool(device_.vkDevice(), cmdPool_, nullptr);
		CP_DEBUG_LOG("command pool destroyed");

		for (int i = 0; i < configuration_.framesInFlight; i++) {
			vkDestroySemaphore(device_.vkDevice(), imageAvailSemaphores_[i], nullptr);
			vkDestroySemaphore(device_.vkDevice(), renderFinishedSemaphores_[i], nullptr);
			vkDestroyFence(device_.vkDevice(), inFlightFences_[i], nullptr);
		}
		CP_DEBUG_LOG("sync objects destroyed");
	}

	void Renderer::init() {
		pipeline_ = std::make_unique<Pipeline>(device_, swapchain_);

		Shader shader(device_, "assets/shadersbin/vert.spv", "assets/shadersbin/frag.spv");
		pipeline_->setShaderStages(shader);
		pipeline_->create();

		renderPass_ = std::make_unique<RenderPass>(device_, swapchain_);

		createFramebuffers();
		
		QueueFamilyIndices queueFamilies = device_.queueFamilies();

		VkCommandPoolCreateInfo cmdPoolInfo{};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkResult cmdPoolResult = vkCreateCommandPool(device_.vkDevice(), &cmdPoolInfo, nullptr, &cmdPool_);
		checkVkResult(cmdPoolResult, "failed to create command pool");

		cmdBuffers_.resize(configuration_.framesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = cmdPool_;
		allocInfo.commandBufferCount = cmdBuffers_.size();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkResult allocResult = vkAllocateCommandBuffers(device_.vkDevice(), &allocInfo, cmdBuffers_.data());
		checkVkResult(allocResult, "failed to allocate command buffers");
	}

	void Renderer::createFramebuffers() {
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
	}

	void Renderer::createSyncObjects() {
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		imageAvailSemaphores_.resize(configuration_.framesInFlight);
		renderFinishedSemaphores_.resize(configuration_.framesInFlight);
		inFlightFences_.resize(configuration_.framesInFlight);

		for (int i = 0; i < configuration_.framesInFlight; i++) {
			VkResult res1 = vkCreateSemaphore(device_.vkDevice(), &semaphoreInfo, nullptr, &imageAvailSemaphores_[i]);
			VkResult res2 = vkCreateSemaphore(device_.vkDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]);
			VkResult res3 = vkCreateFence(device_.vkDevice(), &fenceInfo, nullptr, &inFlightFences_[i]);

			checkVkResult({ res1, res2, res3 }, "failed to create fence or semaphore");
		}
	}

	void Renderer::draw() {
		if (viewportWidth_ <= 0 || viewportHeight_ <= 0) {
			return;
		}

		vkWaitForFences(
			device_.vkDevice(), 1, &inFlightFences_[currentFrame_], VK_TRUE, std::numeric_limits<uint64>::max());

		uint imageIdx = 0;
		VkResult nextImgResult = vkAcquireNextImageKHR(
			device_.vkDevice(),
			swapchain_.vkHandle(),
			std::numeric_limits<uint64>::max(),
			imageAvailSemaphores_[currentFrame_],
			VK_NULL_HANDLE,
			&imageIdx
		);

		if (nextImgResult == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapchain();
			return;
		}
		else if (nextImgResult != VK_SUCCESS && nextImgResult != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		vkResetFences(device_.vkDevice(), 1, &inFlightFences_[currentFrame_]);

		vkResetCommandBuffer(cmdBuffers_[currentFrame_], 0);
		recordCommandBuffer(imageIdx);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailSemaphores_[currentFrame_] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffers_[currentFrame_];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores_[currentFrame_] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkResult submitResult = vkQueueSubmit(device_.graphicsQueue(), 1, &submitInfo, inFlightFences_[currentFrame_]);
		checkVkResult(submitResult, "failed to submit to queue");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		VkSwapchainKHR swapChains[] = { swapchain_.vkHandle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIdx;

		VkResult presentResult = vkQueuePresentKHR(device_.graphicsQueue(), &presentInfo);

		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
			return;
		}
		checkVkResult(presentResult, "failed to present image");

		currentFrame_ = (currentFrame_ + 1) % configuration_.framesInFlight;
	}

	void Renderer::setViewportSize(int width, int height) {
		viewportWidth_ = width;
		viewportHeight_ = height;
	}

	void Renderer::recordCommandBuffer(uint imageIdx) {
		VkCommandBufferBeginInfo bufferBeginInfo{};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkResult beginResult = vkBeginCommandBuffer(cmdBuffers_[currentFrame_], &bufferBeginInfo);
		checkVkResult(beginResult, "failed to begin command buffer");

		VkRenderPassBeginInfo passBeginInfo{};
		passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passBeginInfo.renderPass = renderPass_->vkHandle();
		passBeginInfo.framebuffer = framebuffers_[imageIdx]->vkHandle();
		passBeginInfo.renderArea.extent = swapchain_.extent();
		passBeginInfo.renderArea.offset = { 0, 0 };

		VkClearValue clearColor = { {{ 0.f, 0.f, 0.f, 1.f }} };
		passBeginInfo.clearValueCount = 1;
		passBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(cmdBuffers_[currentFrame_], &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuffers_[currentFrame_], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_->vkHandle());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)viewportWidth_;
		viewport.height = (float)viewportHeight_;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffers_[currentFrame_], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapchain_.extent();
		vkCmdSetScissor(cmdBuffers_[currentFrame_], 0, 1, &scissor);

		vkCmdDraw(cmdBuffers_[currentFrame_], 3, 1, 0, 0);

		vkCmdEndRenderPass(cmdBuffers_[currentFrame_]);

		VkResult endBufferResult = vkEndCommandBuffer(cmdBuffers_[currentFrame_]);
		checkVkResult(endBufferResult, "failed to record command buffer");
	}

	void Renderer::recreateSwapchain() {
		device_.wait();

		framebuffers_.clear();
		swapchain_.destroy();

		swapchain_.create();
		createFramebuffers();
	}
}
