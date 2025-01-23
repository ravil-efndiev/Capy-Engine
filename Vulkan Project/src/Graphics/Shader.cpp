#include "Shader.h"
#include <Application.h>

namespace cp {
	Shader::Shader(
		Device& device,
		const std::filesystem::path& vertexShaderPath,
		const std::filesystem::path& fragmentShaderPath
	) : mDevice(device) {

		mVshData = readFileBin(vertexShaderPath);
		mFshData = readFileBin(fragmentShaderPath);

		vertexShaderModule = createShaderModule(mVshData);
		fragmentShaderModule = createShaderModule(mFshData);
	}

	Shader::~Shader() {
		vkDestroyShaderModule(mDevice.vkDevice(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(mDevice.vkDevice(), fragmentShaderModule, nullptr);
		CP_DEBUG_LOG("shader modules destroyed");
	}

	VkShaderModule Shader::createShaderModule(const std::vector<char>& data) {
		VkShaderModule shader = VK_NULL_HANDLE;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = data.size();
		createInfo.pCode = reinterpret_cast<const uint*>(data.data());

		VkResult result = vkCreateShaderModule(mDevice.vkDevice(), &createInfo, nullptr, &shader);
		checkVkResult(result, "couldnt compile shader module");
		return shader;
	}
}