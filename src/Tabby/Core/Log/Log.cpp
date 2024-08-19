#include <Tabby/Core/Application.h>
#include <Tabby/Core/Log/Log.h>

#if defined(TB_PLATFORM_ANDROID)
#include <spdlog/sinks/android_sink.h>
#endif
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/common.h>

namespace Tabby {
template <typename Mutex>
class ConsoleLogSink : public spdlog::sinks::base_sink<Mutex> {
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        static std::queue<std::pair<spdlog::level::level_enum, std::string>> printQueue;

        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);

        printQueue.push(std::make_pair(msg.level, fmt::to_string(formatted)));

        if (!Application::GetConsole())
            return;

        while (!printQueue.empty()) {
            auto front = printQueue.front();
            switch (front.first) {
            case spdlog::level::trace:
                Application::GetConsole()->System().Log(csys::ItemType::LOG) << front.second;
                break;
            case spdlog::level::info:
                Application::GetConsole()->System().Log(csys::ItemType::INFO) << front.second;
                break;
            case spdlog::level::warn:
                Application::GetConsole()->System().Log(csys::ItemType::WARNING) << front.second;
                break;
            case spdlog::level::err:
                Application::GetConsole()->System().Log(csys::ItemType::ERROR) << front.second;
                break;
            case spdlog::level::critical:
                Application::GetConsole()->System().Log(csys::ItemType::ERROR) << front.second;
                break;
            }

            printQueue.pop();
        }
    }

    void flush_() override
    {
    }
};

void Log::Init()
{
    TB_PROFILE_SCOPE_NAME("Tabby::Log::Init");

    std::vector<spdlog::sink_ptr> logSinks;
#if defined(TB_PLATFORM_ANDROID)
    logSinks.emplace_back(std::make_shared<spdlog::sinks::android_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(std::make_shared<ConsoleLogSink<std::mutex>>());
#else
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Tabby.log", true));
    logSinks.emplace_back(std::make_shared<ConsoleLogSink<std::mutex>>());
#endif

    logSinks[0]->set_pattern("%^[%T] %n: %v%$");
    logSinks[1]->set_pattern("[%T] [%l] %n: %v");

    s_CoreLogger = std::make_shared<spdlog::logger>("Tabby", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_CoreLogger);
    s_CoreLogger->set_level(spdlog::level::trace);
    s_CoreLogger->flush_on(spdlog::level::trace);

    s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(logSinks), end(logSinks));
    spdlog::register_logger(s_ClientLogger);
    s_ClientLogger->set_level(spdlog::level::trace);
    s_ClientLogger->flush_on(spdlog::level::trace);
}
}
