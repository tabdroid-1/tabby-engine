#pragma once

#include "Tabby/Core/Base.h"
#include "Tabby/Core/Log.h"
#include <filesystem>

#ifdef TB_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define TB_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { TB##type##ERROR(msg, __VA_ARGS__); TB_DEBUGBREAK(); } }
	#define TB_INTERNAL_ASSERT_WITH_MSG(type, check, ...) TB_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define TB_INTERNAL_ASSERT_NO_MSG(type, check) TB_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", TB_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define TB_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define TB_INTERNAL_ASSERT_GET_MACRO(...) TB_EXPAND_MACRO( TB_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, TB_INTERNAL_ASSERT_WITH_MSG, TB_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define TB_ASSERT(...) TB_EXPAND_MACRO( TB_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define TB_CORE_ASSERT(...) TB_EXPAND_MACRO( TB_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define TB_ASSERT(...)
	#define TB_CORE_ASSERT(...)
#endif
