#pragma once
#include "include.h"

const char* debugSeverityPrefix(VkDebugUtilsMessageSeverityFlagBitsEXT severity);

void enableVirtualTerminalProcessing();

std::vector<char> readFileBin(const std::string& path);