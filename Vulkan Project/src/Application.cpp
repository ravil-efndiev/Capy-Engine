#include "Application.h"
#include "Graphics/Shader.h"

namespace cp {
	Application::Application() {
		enableVirtualTerminalProcessing();
	}

	Application::~Application() {

	}

	void Application::run() {
		try {
			start();
			update();
		}
		catch (const std::runtime_error& err) {
			std::cerr << "Runtime error: " << err.what() << "\n";
		}
	}

	void Application::start() {
		mContext = std::make_unique<VulkanContext>();

		WindowSpecification winSpec{ mContext->instance(), 1200, 920, "vk project" };
		mWindow = std::make_unique<Window>(mEvtHandler, winSpec);
		mDevice = std::make_unique<Device>(mContext->instance(), mWindow->surface());
		mSwapchain = std::make_unique<Swapchain>(*mDevice, *mWindow);
		mRenderer = std::make_unique<Renderer>(*mDevice, *mSwapchain, mEvtHandler);

	}

	void Application::update() {
		while (!mWindow->shouldClose()) {
			if (mWindow->minimized()) {
				mDevice->wait();
				mWindow->wait();
				continue;
			}
			mWindow->pollEvents();
			mRenderer->draw();
		}

		mDevice->wait();
	}
}
