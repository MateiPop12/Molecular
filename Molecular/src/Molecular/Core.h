#pragma once

#ifdef MOL_PLATFORM_WINDOWS
	#ifdef MOL_BUILD_DLL	
		#define MOLECULAR_API __declspec(dllexport)
	#else
		#define MOLECULAR_API __declspec(dllimport)
	#endif
#else
	#error Molecular only supports Windows!
#endif

#ifdef MOL_ENABLE_ASSERTS
	#define MOL_ASSERT(x, ...){if(!(x) {MOL_ERROR("Assertion failed: {0}" __VA_ARGS__); __debugbreak();}}
	#define MOL_CORE_ASSERT(x, ...){if(!(x) {MOL_CORE_ERROR("Assertion failed: {0}" __VA_ARGS__); __debugbreak();}}
#else
	#define MOL_ASSERT(x, ...)
	#define MOL_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)