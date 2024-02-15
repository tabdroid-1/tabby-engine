#pragma once

#include "Tabby/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <filesystem>
// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace Tabby {
class Log {
public:
    static void Init();

    static Ref<spdlog::logger>& GetCoreLogger()
    {
        return s_CoreLogger;
    }
    static Ref<spdlog::logger>& GetClientLogger()
    {
        return s_ClientLogger;
    }

private:
    static Ref<spdlog::logger> s_CoreLogger;
    static Ref<spdlog::logger> s_ClientLogger;
};
}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
    return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}

// Core log macros
#define TB_CORE_TRACE(...) ::Tabby::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define TB_CORE_INFO(...) ::Tabby::Log::GetCoreLogger()->info(__VA_ARGS__)
#define TB_CORE_WARN(...) ::Tabby::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define TB_CORE_ERROR(...) ::Tabby::Log::GetCoreLogger()->error(__VA_ARGS__)
#define TB_CORE_CRITICAL(...) ::Tabby::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define TB_TRACE(...) ::Tabby::Log::GetClientLogger()->trace(__VA_ARGS__)
#define TB_INFO(...) ::Tabby::Log::GetClientLogger()->info(__VA_ARGS__)
#define TB_WARN(...) ::Tabby::Log::GetClientLogger()->warn(__VA_ARGS__)
#define TB_ERROR(...) ::Tabby::Log::GetClientLogger()->error(__VA_ARGS__)
#define TB_CRITICAL(...) ::Tabby::Log::GetClientLogger()->critical(__VA_ARGS__)
