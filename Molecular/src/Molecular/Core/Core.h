#pragma once

#include <memory>

#ifdef MOL_PLATFORM_WINDOWS
	#if MOL_DYNAMIC_LIBRARY
		#ifdef MOL_BUILD_DLL
			#define MOLECULAR_API __declspec(dllexport)
		#else
			#define MOLECULAR_API __declspec(dllimport)
		#endif
	#else
		#define MOLECULAR_API
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

#define MOL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Molecular
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}