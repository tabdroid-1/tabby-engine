#pragma once

#include "Tabby/Core/PlatformDetection.h"

#include <memory>

#ifdef TB_DEBUG
	#if defined(TB_PLATFORM_WINDOWS)
		#define TB_DEBUGBREAK() __debugbreak()
	#elif defined(TB_PLATFORM_LINUX)
		#include <signal.h>
		#define TB_DEBUGBREAK() raise(SIGTRAP)
	#elif defined(TB_PLATFORM_MACOS)
		#define TB_DEBUGBREAK() __builtin_debugtrap()
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define TB_ENABLE_ASSERTS
#else
	#define TB_DEBUGBREAK()
#endif

#define TB_EXPAND_MACRO(x) x
#define TB_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define TB_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Tabby {

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

#include "Tabby/Core/Log.h"
#include "Tabby/Core/Assert.h"
