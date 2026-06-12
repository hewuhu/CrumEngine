module;
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

export module crumb.core:log;

import std;

export namespace Crumb {

class Log {
public:
    Log() = delete;

    static void Init(std::string_view core_name = "Crumb", std::string_view client_name = "APP") {
        static std::once_flag core_flag, client_flag;
        std::call_once(core_flag, [&] {
            s_CoreLogger = spdlog::stdout_color_mt(std::string(core_name));
            s_CoreLogger->set_level(spdlog::level::trace);
            s_CoreLogger->set_pattern("[%H:%M:%S] [%^%l%$] %v");
        });
        std::call_once(client_flag, [&] {
            s_CilenLogger = spdlog::stdout_color_mt(std::string(client_name));
            s_CilenLogger->set_level(spdlog::level::trace);
            s_CilenLogger->set_pattern("[%H:%M:%S] [%^%l%$] %v");
        });
    }
    
    // 引擎
    template<typename... Args>
    static void Trace(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CoreLogger, spdlog::level::trace, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void Debug(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CoreLogger, spdlog::level::debug, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void Info(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CoreLogger, spdlog::level::info, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void Warn(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CoreLogger, spdlog::level::warn, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void Error(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CoreLogger, spdlog::level::err, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void Critical(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CoreLogger, spdlog::level::critical, fmt, std::forward<Args>(args)...);
    }

    // 客户端
    template<typename... Args>
    static void ClientTrace(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CilenLogger, spdlog::level::trace, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void ClientDebug(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CilenLogger, spdlog::level::debug, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void ClientInfo(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CilenLogger, spdlog::level::info, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void ClientWarn(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CilenLogger, spdlog::level::warn, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void ClientError(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CilenLogger, spdlog::level::err, fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    static void ClientCritical(fmt::format_string<Args...> fmt, Args&&... args) {
        LogTo(s_CilenLogger, spdlog::level::critical, fmt, std::forward<Args>(args)...);
    }

    static void SetLevel(spdlog::level::level_enum lvl) {
        if (s_CoreLogger) s_CoreLogger->set_level(lvl);
    }
    static void SetClientLevel(spdlog::level::level_enum lvl) {
        if (s_CilenLogger) s_CilenLogger->set_level(lvl);
    }

    // 退出时
    static void Shutdown() {
        if (s_CoreLogger) {
            s_CoreLogger->flush();
            s_CoreLogger.reset();
        }
        if (s_CilenLogger) {
            s_CilenLogger->flush();
            s_CilenLogger.reset();
        }
        spdlog::shutdown();
    }

private:
    template<typename... Args>
    static void LogTo(std::shared_ptr<spdlog::logger>& dest,
                      spdlog::level::level_enum lvl,
                      fmt::format_string<Args...> fmt, Args&&... args) {
        if (dest) dest->log(lvl, fmt, std::forward<Args>(args)...);
    }

    inline static std::shared_ptr<spdlog::logger> s_CoreLogger;
    inline static std::shared_ptr<spdlog::logger> s_CilenLogger;
};

} // namespace Crumb
