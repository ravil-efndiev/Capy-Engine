#pragma once
#include <Vulkan/Device.h>
#include <Vulkan/Swapchain.h>
#include <Events/EventHandler.h>
#include "Pipeline.h"
#include "Framebuffer.h"
#include "RenderPass.h"

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
	void finish();

private:
	void init();
	void createSyncObjects();

	void recordCommandBuffer(uint imageIdx);

private:
	RendererConfiguration configuration_;
	std::unique_ptr<Pipeline> pipeline_;
	std::unique_ptr<RenderPass> renderPass_;
	std::vector<std::unique_ptr<Framebuffer>> framebuffers_;
	Device& device_;
	Swapchain& swapchain_;
	VkCommandPool cmdPool_;
	VkCommandBuffer cmdBuffer_;
	VkSemaphore imageAvailSemaphore_;
	VkSemaphore renderFinishedSemaphore_;
	VkFence	inFlightFence_;
};

