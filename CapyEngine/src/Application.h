#pragma once
#include "Vulkan/VulkanContext.h"
#include "Vulkan/Window.h"
#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"
#include "Graphics/Renderer.h"
#include "Events/EventHandler.h"
#include "API/PerspectiveCamera.h"
#include "API/Transform.h"
#include "Graphics/Shader.h"
#include "API/Time.h"
#include "API/Input.h"

namespace cp {
	class Application {
	public:
		template <class AppT>
		static Application& create() {
			sInstance = std::make_unique<AppT>();
			return *sInstance;
		}
		static Application& get();

		Application();
		virtual ~Application() = default;
		void run();

		virtual void start() = 0;
		virtual void update() = 0;

		VulkanContext& context() { return *mContext; }
		Window& window() { return *mWindow; }
		Swapchain& swapchain() { return *mSwapchain; }
		Device& device() { return *mDevice; }
		EventHandler& eventHandler() { return mEvtHandler; }

	private:
		std::unique_ptr<VulkanContext> mContext;
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<Device> mDevice;
		std::unique_ptr<Swapchain> mSwapchain;
		EventHandler mEvtHandler;

		static std::unique_ptr<Application> sInstance;
	};
}
