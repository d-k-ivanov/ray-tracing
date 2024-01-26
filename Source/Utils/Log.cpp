#include "Log.h"

#include <Utils/Filesystem.h>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init()
{
    std::vector<spdlog::sink_ptr> logSinks;
    logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    logSinks.emplace_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(ThisExecutableLocation() + "/Logs/ray-tracing.log", 1024 * 1024 * 100, 5));

    logSinks[0]->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] [thread %t] [%s:%# %!] %v");
    logSinks[1]->set_pattern("[%Y-%m-%d %H:%M:%S] [%n] [%^%l%$] [thread %t] [%s:%# %!] %v");

    s_Logger = std::make_shared<spdlog::logger>("Ray-Tracing", begin(logSinks), end(logSinks));
    register_logger(s_Logger);
    s_Logger->set_level(spdlog::level::trace);
    s_Logger->flush_on(spdlog::level::trace);
}

void Log::SetLogLevelWarn()
{
    s_Logger->set_level(spdlog::level::warn);
}
