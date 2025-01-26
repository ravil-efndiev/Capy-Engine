#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>
#include <Events/EventHandler.h>
#include "Pipeline.h"
#include "Framebuffer.h"
#include "RenderPass.h"
#include "Buffers.h"
#include "Mesh.h"
#include "Uniforms.h"
#include <API/Transform.h>

namespace cp {
	struct PipelineHandle {
		uint id = (uint)(-1); // will be more then size of a vector (uint32_t max)
	};

	struct RendererConfiguration {
		uint framesInFlight = 2;
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

		PipelineHandle addPipelineConfiguration(PipelineConfiguration& config);
		void usePipeline(PipelineHandle handle);

		void begin();
		void end();
		void submitMesh(const Mesh<PositionColorVertex>& mesh, const Transform& tf);
		void submitMesh(const Mesh<SpriteVertex>& mesh, const Transform& tf);

		void setViewportSize(int width, int height);
		void setProjView(const glm::mat4& projection, const glm::mat4& view);

		glm::vec2 viewportSize() const { return { mViewportWidth, mViewportHeight }; }

	private:
		void init();
		void createFramebuffers();
		void createSyncObjects();
		void createDescriptorSets();

		void bindAndDrawBuffers(const VertexBuffer& vb, const IndexBuffer& ib);
		void updateModelMatrix(const glm::mat4& model);
		void recreateSwapchain();

	private:
		RendererConfiguration mConfig;
		std::vector<std::unique_ptr<Pipeline>> mPipelines;
		PipelineHandle mCurrentPipeline{};

		std::unique_ptr<RenderPass> mRenderPass;
		std::vector<Framebuffer> mFramebuffers;
		
		Device& mDevice;
		Swapchain& mSwapchain;
		
		VkCommandPool mCmdPool;
		VkDescriptorPool mDescriptorPool;

		std::vector<VkCommandBuffer> mCmdBuffers;
		std::vector<VkSemaphore> mImageAvailSemaphores;
		std::vector<VkSemaphore> mRenderFinishedSemaphores;
		std::vector<VkFence> mInFlightFences;

		std::vector<VkDescriptorSet> mDescriptorSets;
		std::vector<std::unique_ptr<UniformBuffer<ProjViewUBO>>> mMatrixUniformBuffers;

		int mViewportWidth, mViewportHeight;
		uint mCurrentFrame = 0;
		uint mImageIdx = 0;
	};
}
