#include "Renderer.h"

namespace cp {
	Renderer::Renderer(
		Device& device,
		Swapchain& swapchain,
		EventHandler& evtHandler,
		const RendererConfiguration& config
	)
		: mDevice(device), mSwapchain(swapchain), mConfig(config) {

		mViewportWidth = mSwapchain.extent().width;
		mViewportHeight = mSwapchain.extent().height;

		init();
		createSyncObjects();

		evtHandler.subscribeToResizeEvt([&](int width, int height) {
			setViewportSize(width, height);
		});
	}

	Renderer::~Renderer() {
		vkDestroyCommandPool(mDevice.vkDevice(), mCmdPool, nullptr);
		vkDestroyDescriptorPool(mDevice.vkDevice(), mDescriptorPool, nullptr);
		CP_DEBUG_LOG("command pool destroyed");

		for (size_t i = 0; i < mConfig.framesInFlight; i++) {
			vkDestroySemaphore(mDevice.vkDevice(), mImageAvailSemaphores[i], nullptr);
			vkDestroySemaphore(mDevice.vkDevice(), mRenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(mDevice.vkDevice(), mInFlightFences[i], nullptr);
		}
		CP_DEBUG_LOG("sync objects destroyed");
	}

	PipelineHandle Renderer::addPipelineConfiguration(
		PipelineConfiguration& config, 
		bool useDefaultDescriptorBindings
	) {
		if (useDefaultDescriptorBindings) {
			config.descriptorSetBindings = {
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT } // Matrix uniform
			};
		}
		mPipelines.push_back(std::make_unique<Pipeline>(mDevice, mSwapchain, config));
		return { (uint)mPipelines.size() - 1 };
	}

	void Renderer::usePipeline(PipelineHandle handle) {
		CP_ASSERT(handle.id < mPipelines.size(), "invalid pipeline handle");
		mCurrentPipeline = handle;

		if (mCurrentPipeline.id == 0) {
			createDescriptorSets();
		}
	}

	void Renderer::init() {
		mRenderPass = std::make_unique<RenderPass>(mDevice, mSwapchain);

		createFramebuffers();
		
		QueueFamilyIndices queueFamilies = mDevice.queueFamilies();

		VkCommandPoolCreateInfo cmdPoolInfo{};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = queueFamilies.graphicsFamily.value();
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		VkResult cmdPoolResult = vkCreateCommandPool(mDevice.vkDevice(), &cmdPoolInfo, nullptr, &mCmdPool);
		checkVkResult(cmdPoolResult, "failed to create command pool");

		mCmdBuffers.resize(mConfig.framesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = mCmdPool;
		allocInfo.commandBufferCount = (uint)mCmdBuffers.size();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkResult allocResult = vkAllocateCommandBuffers(mDevice.vkDevice(), &allocInfo, mCmdBuffers.data());
		checkVkResult(allocResult, "failed to allocate command buffers");

		for (uint i = 0; i < mConfig.framesInFlight; i++) {
			mMatrixUniformBuffers.push_back(std::make_unique<UniformBuffer<MatrixUBO>>(mDevice));
		}
	}

	void Renderer::createFramebuffers() {
		std::vector<Swapchain::Image> swapchainImages = mSwapchain.images();
		VkExtent2D extent = mSwapchain.extent();

		mFramebuffers.reserve(swapchainImages.size());
		for (const auto& image : swapchainImages) {
			FramebufferSpecification framebufferSpec{};
			framebufferSpec.width = extent.width;
			framebufferSpec.height = extent.height;
			framebufferSpec.attachment = image.view;
			framebufferSpec.pRenderPass = mRenderPass.get();

			mFramebuffers.push_back(std::make_unique<Framebuffer>(mDevice, framebufferSpec));
		}
	}

	void Renderer::createSyncObjects() {
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		mImageAvailSemaphores.resize(mConfig.framesInFlight);
		mRenderFinishedSemaphores.resize(mConfig.framesInFlight);
		mInFlightFences.resize(mConfig.framesInFlight);

		for (uint i = 0; i < mConfig.framesInFlight; i++) {
			VkResult res1 = vkCreateSemaphore(mDevice.vkDevice(), &semaphoreInfo, nullptr, &mImageAvailSemaphores[i]);
			VkResult res2 = vkCreateSemaphore(mDevice.vkDevice(), &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]);
			VkResult res3 = vkCreateFence(mDevice.vkDevice(), &fenceInfo, nullptr, &mInFlightFences[i]);

			checkVkResult({ res1, res2, res3 }, "failed to create fence or semaphore");
		}
	}

	void Renderer::createDescriptorSets() {
		VkDescriptorPoolSize poolSize{};
		poolSize.descriptorCount = mConfig.framesInFlight;
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = mConfig.framesInFlight;

		VkResult poolResult = vkCreateDescriptorPool(mDevice.vkDevice(), &poolInfo, nullptr, &mDescriptorPool);
		checkVkResult(poolResult, "failed to create descriptor pool");

		std::vector<VkDescriptorSetLayout> layouts(
			mConfig.framesInFlight, 
			mPipelines[mCurrentPipeline.id]->descriptorSetLayout()
		);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = mDescriptorPool;
		allocInfo.descriptorSetCount = mConfig.framesInFlight;
		allocInfo.pSetLayouts = layouts.data();

		mDescriptorSets.resize(mConfig.framesInFlight);
		VkResult setResult = vkAllocateDescriptorSets(mDevice.vkDevice(), &allocInfo, mDescriptorSets.data());
		checkVkResult(setResult, "failed to allocate descriptor sets");

		for (uint i = 0; i < mConfig.framesInFlight; i++) {
			VkDescriptorBufferInfo descBufferInfo{};
			descBufferInfo.buffer = mMatrixUniformBuffers[i]->vkHandle();
			descBufferInfo.offset = 0;
			descBufferInfo.range = mMatrixUniformBuffers[i]->uboSize();

			VkWriteDescriptorSet descWrite{};
			descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descWrite.dstSet = mDescriptorSets[i];
			descWrite.dstBinding = 0;
			descWrite.dstArrayElement = 0;
			descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descWrite.descriptorCount = 1;
			descWrite.pBufferInfo = &descBufferInfo;

			vkUpdateDescriptorSets(mDevice.vkDevice(), 1, &descWrite, 0, nullptr);
		}
	}

	void Renderer::submitMesh(const Mesh<PositionColorVertex>& mesh) {
		CP_ASSERT(
			mPipelines[mCurrentPipeline.id]->configuration().vertexType == PipelineConfiguration::PositionColorVertex,
			"cannot submit mesh with vertex type PositionColorVertex with different pipeline configuration"
		);
		draw(mesh.vertexBuffer(), mesh.indexBuffer());
	}

	void Renderer::submitMesh(const Mesh<SpriteVertex>& mesh) {
		CP_ASSERT(
			mPipelines[mCurrentPipeline.id]->configuration().vertexType == PipelineConfiguration::TexCoordVertex,
			"cannot submit mesh with vertex type SpriteVertex with different pipeline configuration"
		);
		draw(mesh.vertexBuffer(), mesh.indexBuffer());
	}

	void Renderer::draw(VertexBuffer& vb, IndexBuffer& ib) {
		if (mViewportWidth <= 0 || mViewportHeight <= 0) {
			return;
		}

		vkWaitForFences(mDevice.vkDevice(), 1, &mInFlightFences[mCurrentFrame], VK_TRUE, std::numeric_limits<uint64>::max());

		uint imageIdx = 0;
		VkResult nextImgResult = vkAcquireNextImageKHR(
			mDevice.vkDevice(),
			mSwapchain.vkHandle(),
			std::numeric_limits<uint64>::max(),
			mImageAvailSemaphores[mCurrentFrame],
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

		vkResetFences(mDevice.vkDevice(), 1, &mInFlightFences[mCurrentFrame]);

		vkResetCommandBuffer(mCmdBuffers[mCurrentFrame], 0);
		recordCommandBuffer(imageIdx, vb, ib);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { mImageAvailSemaphores[mCurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCmdBuffers[mCurrentFrame];

		VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VkResult submitResult = vkQueueSubmit(mDevice.graphicsQueue(), 1, &submitInfo, mInFlightFences[mCurrentFrame]);
		checkVkResult(submitResult, "failed to submit to queue");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		VkSwapchainKHR swapChains[] = { mSwapchain.vkHandle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIdx;

		VkResult presentResult = vkQueuePresentKHR(mDevice.graphicsQueue(), &presentInfo);

		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
			recreateSwapchain();
			return;
		}
		checkVkResult(presentResult, "failed to present image");

		mCurrentFrame = (mCurrentFrame + 1) % mConfig.framesInFlight;
	}

	void Renderer::setViewportSize(int width, int height) {
		mViewportWidth = width;
		mViewportHeight = height;
	}

	void Renderer::setMatrixUBO(const MatrixUBO& ubo) {
		mMatrixUniformBuffers[mCurrentFrame]->update(ubo);
	}

	void Renderer::recordCommandBuffer(uint imageIdx, VertexBuffer& vb, IndexBuffer& ib) {
		VkCommandBufferBeginInfo bufferBeginInfo{};
		bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkResult beginResult = vkBeginCommandBuffer(mCmdBuffers[mCurrentFrame], &bufferBeginInfo);
		checkVkResult(beginResult, "failed to begin command buffer");

		VkRenderPassBeginInfo passBeginInfo{};
		passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passBeginInfo.renderPass = mRenderPass->vkHandle();
		passBeginInfo.framebuffer = mFramebuffers[imageIdx]->vkHandle();
		passBeginInfo.renderArea.extent = mSwapchain.extent();
		passBeginInfo.renderArea.offset = { 0, 0 };

		VkClearValue clearColor = { {{ 0.f, 0.f, 0.f, 1.f }} };
		passBeginInfo.clearValueCount = 1;
		passBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(mCmdBuffers[mCurrentFrame], &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(mCmdBuffers[mCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelines[mCurrentPipeline.id]->vkHandle());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)mViewportWidth;
		viewport.height = (float)mViewportHeight;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(mCmdBuffers[mCurrentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = mSwapchain.extent();
		vkCmdSetScissor(mCmdBuffers[mCurrentFrame], 0, 1, &scissor);

		VkBuffer vertexBuffers[] = { vb.vkHandle() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(mCmdBuffers[mCurrentFrame], 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(mCmdBuffers[mCurrentFrame], ib.vkHandle(), 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(
			mCmdBuffers[mCurrentFrame],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			mPipelines[mCurrentPipeline.id]->layout(),
			0, 1,
			&mDescriptorSets[mCurrentFrame],
			0, nullptr
		);

		vkCmdDrawIndexed(mCmdBuffers[mCurrentFrame], (uint)ib.indexCount(), 1, 0, 0, 0);

		vkCmdEndRenderPass(mCmdBuffers[mCurrentFrame]);

		VkResult endBufferResult = vkEndCommandBuffer(mCmdBuffers[mCurrentFrame]);
		checkVkResult(endBufferResult, "failed to record command buffer");
	}

	void Renderer::recreateSwapchain() {
		mDevice.wait();

		mFramebuffers.clear();
		mSwapchain.destroy();

		mSwapchain.create();
		createFramebuffers();
	}
}
