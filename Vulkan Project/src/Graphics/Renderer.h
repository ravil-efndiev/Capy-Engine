#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>
#include <Events/EventHandler.h>
#include "Pipeline.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "Buffers.h"

namespace cp {
	struct Mesh {
		VertexBuffer* vertexBuffer;
		IndexBuffer* indexBuffer;
	};

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

		void submitMesh(const Mesh& vb);

		void setViewportSize(int width, int height);

	private:
		void init();
		void createFramebuffers();
		void createSyncObjects();

		void draw(const Mesh& mesh);

		void recordCommandBuffer(uint imageIdx, const Mesh& mesh);
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
