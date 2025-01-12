#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <optional>
#include <unordered_set>
#include <fstream>
#include <sstream>

#ifdef WIN32
	#define NOMINMAX
	#include <Windows.h>
#endif

#ifndef NDEBUG
	#define RASSERT(cond, err) if (!(cond)) { \
		std::cerr << "Assertion failed in " << __FUNCTION__ << ", details:\n" << err << "\n"; \
		std::abort(); \
	}
	#define RDEBUG_VULKAN(severity, message) printf("%s %s\n", severity, message)
	#define RDEBUG_LOG(fmt, ...) printf(("\033[36m[Info]\033[0m " + std::string(fmt) + "\n").c_str(), __VA_ARGS__)
	#define RDEBUG_ERROR(fmt, ...) fprintf(stderr, ("\033[31m[Error]\033[0m" + std::string(fmt) + "\n").c_str(), __VA_ARGS__)
	constexpr bool VALIDATION_LAYERS_ENABLED = true;
#else 
	#define RASSERT()
	#define RDEBUG_LOG(fmt, ...)
	#define RDEBUG_ERROR(fmt, ...)
	#define RDEBUG_VULKAN(severity, message)
	constexpr bool VALIDATION_LAYERS_ENABLED = false;
#endif

typedef uint32_t uint;
typedef uint16_t uint16;
typedef uint64_t uint64;
typedef int16_t int16;
typedef int64_t int64;

struct QueueFamilyIndices {
	std::optional<uint> graphicsFamily;
	std::optional<uint> presentFamily;
	bool complete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};
