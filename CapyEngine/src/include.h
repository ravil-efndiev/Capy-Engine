#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <optional>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <functional>
#include <memory>
#include <filesystem>

#ifdef _MSC_VER
	#define NOMINMAX
	#include <Windows.h>
	#define CP_VA_ARGS(...) , __VA_ARGS__
#elif defined(__GNUC__) || defined(__clang__)
	#define CP_VA_ARGS(...) , ##__VA_ARGS__
#endif

#define CP_DEBUG

#ifdef CP_DEBUG
	#define CP_ASSERT(cond, err) if (!(cond)) { \
		std::cerr << "Assertion failed in function " << __FUNCTION__ << " in file " << __FILE__ << ", details:\n" << err << "\n"; \
		std::abort(); \
	}
	#define CP_DEBUG_VULKAN(severity, message) printf("%s %s\n", severity, message)
	#define CP_DEBUG_LOG(fmt, ...) printf("\033[36m[Info]\033[0m " fmt "\n" CP_VA_ARGS(__VA_ARGS__))
	#define CP_DEBUG_ERROR(fmt, ...) fprintf(stderr, "\033[31m[Error]\033[0m " fmt "\n" CP_VA_ARGS(__VA_ARGS__))
#else 
	#define CP_ASSERT()
	#define CP_DEBUG_LOG(fmt, ...)
	#define CP_DEBUG_ERROR(fmt, ...)
	#define CP_DEBUG_VULKAN(severity, message)
#endif

#include "Enums.h"

namespace cp {
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
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

#ifdef CP_DEBUG
	constexpr bool gValidationLayersEnabled = true;
#else
	constexpr bool gValidationLayersEnabled = false;
#endif

	struct Constants {
		const uint engineVersion = VK_MAKE_VERSION(1, 0, 0);
		const std::filesystem::path assetsDir = "assets";
		const std::filesystem::path spirvDir = assetsDir / "shadersbin";
	};

	inline const Constants gConstants{};
}
