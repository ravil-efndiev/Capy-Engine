#include "Application.h"
#include "Graphics/Shader.h"
#include "API/Time.h"

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

		mRenderer = std::make_unique<Renderer>(*mDevice, *mSwapchain, mEvtHandler);

		Shader shader(*mDevice, gConstants.spirvDir / "vert.spv", gConstants.spirvDir / "frag.spv");

		PipelineConfiguration pipelineConfig{};
		pipelineConfig.culling = VK_CULL_MODE_BACK_BIT;
		pipelineConfig.pShader = &shader;
		//rendererConfig.pipelineConfig.polygonMode = VK_POLYGON_MODE_LINE;
		
		PipelineHandle pipeline = mRenderer->addPipelineConfiguration(pipelineConfig);
		mRenderer->usePipeline(pipeline);

		std::vector<PositionColorVertex> vertices = {
			{{-0.5f, -0.5f, 0.f}, {1.0f, 1.0f, 1.0f, 1.f}},
			{{0.5f, -0.5f, 0.f}, {1.0f, 1.0f, 0.0f, 1.f}},
			{{0.5f, 0.5f, 0.f}, {0.0f, 1.0f, 1.0f, 1.f}},
			{{-0.5f, 0.5f, 0.f}, {1.0f, 0.0f, 1.0f, 1.f}}
		};

		std::vector<uint16> indices = {
			0, 1, 2, 2, 3, 0
		};

		mMesh = std::make_unique<Mesh<PositionColorVertex>>(*mDevice, vertices, indices);
	}

	void Application::update() {
		while (!mWindow->shouldClose()) {
			if (mWindow->minimized()) {
				mWindow->wait();
				continue;
			}

			Time::update();
			mWindow->pollEvents();
			
			mRenderer->submitMesh(*mMesh);

			//CP_DEBUG_LOG("%f", 1.f / Time::dt());
		}

		ResourceManager::cleanup(*mDevice);
		mDevice->wait();
	}
}
