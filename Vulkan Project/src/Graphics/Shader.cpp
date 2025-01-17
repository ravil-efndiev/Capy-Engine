#include "Shader.h"
#include <Application.h>

Shader::Shader(Device& device, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	: device_(device) {

	vshData_ = readFileBin(vertexShaderPath);
	fshData_ = readFileBin(fragmentShaderPath);

	vertexShaderModule = createShaderModule(vshData_);
	fragmentShaderModule = createShaderModule(fshData_);
}

Shader::~Shader() {
	vkDestroyShaderModule(device_.vkDevice(), vertexShaderModule, nullptr);
	vkDestroyShaderModule(device_.vkDevice(), fragmentShaderModule, nullptr);
}

VkShaderModule Shader::createShaderModule(const std::vector<char>& data) {
	VkShaderModule shader = VK_NULL_HANDLE;

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = data.size();
	createInfo.pCode = reinterpret_cast<const uint*>(data.data());

	VkResult res = vkCreateShaderModule(device_.vkDevice(), &createInfo, nullptr, &shader);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("couldnt compile shader module");
	}
	return shader;
}
