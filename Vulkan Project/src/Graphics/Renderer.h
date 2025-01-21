#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>
#include <Events/EventHandler.h>
#include "Pipeline.h"
#include "Framebuffer.h"
#include "RenderPass.h"

namespace cp {
	struct RendererConfiguration {
		uint framesInFlight = 2;

		PipelineConfiguration pipelineConfig{};
	};

	class Renderer {
	public:
		Renderer(
			Device& device,
			Swapchain& swpachain,
			EventHandler& evtHandler,
			const RendererConfiguration& config = {}
		);
		~Renderer();

		void draw();
		void setViewportSize(int width, int height);

	private:
		void init();
		void createFramebuffers();
		void createSyncObjects();

		void recordCommandBuffer(uint imageIdx);
		void recreateSwapchain();

	private:
		RendererConfiguration mConfig;
		std::unique_ptr<Pipeline> mPipeline;
		std::unique_ptr<RenderPass> mRenderPass;
		std::vector<std::unique_ptr<Framebuffer>> mFramebuffers;
		Device& mDevice;
		Swapchain& mSwapchain;
		VkCommandPool mCmdPool;
		std::vector<VkCommandBuffer> mCmdBuffers;
		std::vector<VkSemaphore> mImageAvailSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;

		int mViewportWidth, mViewportHeight;
		uint mCurrentFrame = 0;
	};
}
