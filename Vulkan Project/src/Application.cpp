#include "Application.h"
#include "Graphics/Shader.h"

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
	context_ = std::make_unique<VulkanContext>();

	WindowSpecification winSpec{ context_->instance(), 1200, 920, "vk project" };
	window_ = std::make_unique<Window>(evtHandler_, winSpec);
	device_ = std::make_unique<Device>(context_->instance(), window_->surface());
	swapchain_ = std::make_unique<Swapchain>(*device_, *window_);
	renderer_ = std::make_unique<Renderer>(*device_, *swapchain_, evtHandler_);

}
	
void Application::update() {
	while (!window_->shouldClose()) {
		window_->pollEvents();
		renderer_->draw();
	}
	
	renderer_->finish();
}
