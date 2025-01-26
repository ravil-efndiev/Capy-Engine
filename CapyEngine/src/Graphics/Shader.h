#pragma once
#include <Utils.h>
#include <Vulkan/Device.h>

namespace cp {
	class Shader {
	public:
		Shader(const std::filesystem::path& vertexShaderPath, const std::filesystem::path& fragmentShaderPath);
		~Shader();

		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;

	private:
		VkShaderModule createShaderModule(const std::vector<char>& data);

	private:
		Device& mDevice;
		std::vector<char> mVshData;
		std::vector<char> mFshData;
	};
}
