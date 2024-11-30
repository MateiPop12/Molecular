#pragma once

#include "Core.h"

#pragma warning(push, 0)
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#pragma warning(pop)

namespace Molecular
{
	class MOLECULAR_API Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

//Core log macros
#define MOL_CORE_FATAL(...) ::Molecular::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define MOL_CORE_ERROR(...) ::Molecular::Log::GetCoreLogger()->error(__VA_ARGS__)
#define MOL_CORE_WARN(...)  ::Molecular::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define MOL_CORE_INFO(...)  ::Molecular::Log::GetCoreLogger()->info(__VA_ARGS__)
#define MOL_CORE_TRACE(...) ::Molecular::Log::GetCoreLogger()->trace(__VA_ARGS__)

//Client log macros
#define MOL_FATAL(...) ::Molecular::Log::GetClientLogger()->critical(__VA_ARGS__)
#define MOL_ERROR(...) ::Molecular::Log::GetClientLogger()->error(__VA_ARGS__)
#define MOL_WARN(...)  ::Molecular::Log::GetClientLogger()->warn(__VA_ARGS__)
#define MOL_INFO(...)  ::Molecular::Log::GetClientLogger()->info(__VA_ARGS__)
#define MOL_TRACE(...) ::Molecular::Log::GetClientLogger()->trace(__VA_ARGS__)