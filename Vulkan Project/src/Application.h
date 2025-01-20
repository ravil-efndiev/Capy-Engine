#pragma once
#include "Vulkan/VulkanContext.h"
#include "Vulkan/Window.h"
#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"
#include "Graphics/Renderer.h"
#include "Events/EventHandler.h"

namespace cp {
	class Application {
	public:
		Application();
		~Application();
		void run();

		VulkanContext& context() { return *context_; }
		Window& window() { return *window_; }
		Device& device() { return *device_; }
		Swapchain& swapchain() { return *swapchain_; }

	private:
		void start();
		void update();

	private:
		std::unique_ptr<VulkanContext> context_;
		std::unique_ptr<Window> window_;
		std::unique_ptr<Device> device_;
		std::unique_ptr<Swapchain> swapchain_;
		std::unique_ptr<Renderer> renderer_;
		EventHandler evtHandler_;
	};
}
