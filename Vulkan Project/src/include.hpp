#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>

#ifndef NDEBUG
	#define RASSERT(cond, err) if (!(cond)) { \
		std::cerr << "Assertion failed in " << __FUNCTION__ << ", details:\n" << err << "\n"; \
		std::abort(); \
	}
	#define RDEBUG_LOG(fmt, ...) printf((std::string(fmt) + "\n").c_str(), __VA_ARGS__) 
#else 
	#define RASSERT()
	#define RDEBUG_LOG(fmt, ...)
#endif

#ifdef NDEBUG
	#define RVALIDATION_LAYERS_ENABLED false
#else
	#define RVALIDATION_LAYERS_ENABLED true
#endif


typedef uint32_t uint;
typedef uint16_t uint16;
typedef uint64_t uint64;
typedef int16_t int16;
typedef int64_t int64;

