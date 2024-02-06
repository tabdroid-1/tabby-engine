#include "Tabby/Core/Log.h"
#include "tbpch.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined(TB_PLATFORM_ANDROID)
#include "spdlog/sinks/android_sink.h"
#endif

namespace Tabby {

Ref<spdlog::logger> Log::s_CoreLogger;
Ref<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
    std::vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#if !defined(TB_PLATFORM_ANDROID)
    logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Tabby.log", true));
#endif

    logSinks[0]->set_pattern("%^[%T] %n: %v%$");
    logSinks[1]->set_pattern("[%T] [%l] %n: %v");

#if defined(TB_PLATFORM_ANDROID)
    s_CoreLogger = std::make_shared<spdlog::logger>("Tabby", begin(logSinks), end(logSinks));
    s_CoreLogger->sinks().emplace_back(std::make_shared<spdlog::sinks::android_sink_mt>("Tabby"));
#else
    s_CoreLogger = std::make_shared<spdlog::logger>("Tabby", begin(logSinks), end(logSinks));
#endif
    spdlog::register_logger(s_CoreLogger);
    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::trace);

#if defined(TB_PLATFORM_ANDROID)
    s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
    s_ClientLogger->sinks().emplace_back(std::make_shared<spdlog::sinks::android_sink_mt>("Tabby"));
#else
    s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
#endif
    spdlog::register_logger(s_ClientLogger);
    s_ClientLogger->set_level(spdlog::level::trace);
    s_ClientLogger->flush_on(spdlog::level::trace);
}

}
