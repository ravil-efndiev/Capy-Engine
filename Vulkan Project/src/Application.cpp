#include "Application.h"
#include "Vulkan/Shader.h"

Application::Application() {
	enableVirtualTerminalProcessing();
}

Application::~Application() {

}

void Application::run() {
	start();
	update();
}

void Application::start() {
	Shader shader("assets/shaders/test.vert", "");

	context_ = std::make_unique<VulkanContext>();
	window_ = std::make_unique<Window>(context_->instance(), 1200, 920, "vk project");
	device_ = std::make_unique<Device>(context_->instance(), window_->surface());
	swapchain_ = std::make_unique<Swapchain>(*device_, *window_);
}
	
void Application::update() {
	while (!window_->shouldClose()) {
		window_->pollEvents();
	}
}
