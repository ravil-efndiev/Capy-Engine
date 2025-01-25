#pragma once
#include "Vulkan/VulkanContext.h"
#include "Vulkan/Window.h"
#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"
#include "Graphics/Renderer.h"
#include "Events/EventHandler.h"
#include "API/PerspectiveCamera.h"
#include "API/Transform.h"

namespace cp {
	class Application {
	public:
		Application();
		virtual ~Application() = default;
		void run();

		VulkanContext& context() { return *mContext; }
		Window& window() { return *mWindow; }
		Device& device() { return *mDevice; }
		Swapchain& swapchain() { return *mSwapchain; }

	private:
		void start();
		void update();

	private:
		std::unique_ptr<VulkanContext> mContext;
		std::unique_ptr<Window> mWindow;
		std::unique_ptr<Device> mDevice;
		std::unique_ptr<Swapchain> mSwapchain;
		std::unique_ptr<Renderer> mRenderer;
		std::unique_ptr<Mesh<PositionColorVertex>> mMesh;
		EventHandler mEvtHandler;
		PerspectiveCamera mCamera{glm::vec3(0.f, 0.f, 2.f), 70.f};
		Transform mMeshTf;
		Transform mMeshTf2;
	};
}
