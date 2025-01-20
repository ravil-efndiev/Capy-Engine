#pragma once
#include <Utils.h>
#include <Vulkan/Device.h>

namespace cp {
	class Shader {
	public:
		Shader(Device& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		~Shader();

		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;

	private:
		VkShaderModule createShaderModule(const std::vector<char>& data);

	private:
		Device& device_;
		std::vector<char> vshData_;
		std::vector<char> fshData_;
	};
}
