#pragma once
#include "include.h"

namespace cp {
	const char* debugSeverityPrefix(VkDebugUtilsMessageSeverityFlagBitsEXT severity);

	void enableVirtualTerminalProcessing();

	std::vector<char> readFileBin(const std::filesystem::path& path);

	void checkVkResult(VkResult result, std::string_view errorMessage);
	void checkVkResult(const std::initializer_list<VkResult>& results, std::string_view errorMessage);
}
