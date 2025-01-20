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
		RendererConfiguration configuration_;
		std::unique_ptr<Pipeline> pipeline_;
		std::unique_ptr<RenderPass> renderPass_;
		std::vector<std::unique_ptr<Framebuffer>> framebuffers_;
		Device& device_;
		Swapchain& swapchain_;
		VkCommandPool cmdPool_;
		std::vector<VkCommandBuffer> cmdBuffers_;
		std::vector<VkSemaphore> imageAvailSemaphores_;
		std::vector<VkSemaphore> renderFinishedSemaphores_;
		std::vector<VkFence> inFlightFences_;

		int viewportWidth_, viewportHeight_;
		uint currentFrame_ = 0;
	};
}
