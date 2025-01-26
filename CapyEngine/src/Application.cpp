#include "Application.h"

namespace cp {
	std::unique_ptr<Application> Application::sInstance;

	Application& Application::get() {
		return *sInstance;
	}

	Application::Application() {
		enableVirtualTerminalProcessing();
	}

	void Application::run() {
		try {
			Input::init(mEvtHandler);
			ApplicationConfiguration appConfig{};
			appConfig.applicationName = "Ligma app";
			mContext = std::make_unique<VulkanContext>(appConfig);

			WindowSpecification winSpec{};
			winSpec.instance = mContext->instance();
			winSpec.title = "Ligma app";
			winSpec.width = 1200;
			winSpec.height = 800;
			mWindow = std::make_unique<Window>(mEvtHandler, winSpec);
			mDevice = std::make_unique<Device>(mContext->instance(), mWindow->surface());
			mSwapchain = std::make_unique<Swapchain>(*mDevice, *mWindow);

			ResourceManager::init(*mDevice);

			start();
			while (!mWindow->shouldClose()) {
				if (mWindow->minimized()) {
					mWindow->wait();
					continue;
				}

				Time::update();
				mWindow->pollEvents();

				update();
			}

			ResourceManager::cleanup(*mDevice);
			mDevice->wait();
		}
		catch (const std::runtime_error& err) {
			std::cerr << "Runtime error: " << err.what() << "\n";
		}
	}
}
