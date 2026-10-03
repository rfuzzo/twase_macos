#include "stdafx.hpp"

#include "Utils.hpp"
#include "Config.hpp"
#include "Paths.hpp"

#include <CoreFoundation/CoreFoundation.h>
#include <sys/sysctl.h>
#include <unistd.h>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Utils::CreateLogger(const std::string_view aLogName, const std::string_view aFilename,
                                                    const Paths& aPaths, const Config& aConfig)
{
    try
    {
        auto dir = aPaths.GetLogsDir();

        std::error_code err;
        std::filesystem::create_directories(dir, err);
        if (err)
        {
            SHOW_MESSAGE_BOX_AND_EXIT_FILE_LINE("An error occurred while creating the logs directory:\n{}\n\nDirectory: {}",
                                                err.message(), dir.string());
            return nullptr;
        }

        constexpr auto oneMbInB = 1024 * 1024;

        const auto& loggingConfig = aConfig.GetLogging();
        size_t maxFiles = loggingConfig.maxFiles;
        size_t maxFileSize = static_cast<size_t>(loggingConfig.maxFileSize) * oneMbInB;

        auto file = dir / aFilename;
        auto logger = spdlog::rotating_logger_mt(std::string(aLogName), file.string(), maxFileSize, maxFiles, true);
        logger->set_level(loggingConfig.level);
        logger->flush_on(loggingConfig.flushOn);

        // mirror the log to stdout, visible when the game is started from a terminal
        if (aConfig.GetDev().hasConsole)
        {
            logger->sinks().push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        }

        return logger;
    }
    catch (const std::exception& e)
    {
        SHOW_MESSAGE_BOX_AND_EXIT_FILE_LINE("An exception occurred while creating the logger:\n{}", e.what());
    }

    return nullptr;
}

std::string Utils::FormatCurrentTimestamp()
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::tm tm;
    localtime_r(&now, &tm);

    return fmt::format("{:04d}-{:02d}-{:02d}-{:02d}-{:02d}-{:02d}", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                       tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void Utils::ShowMessageBox(const std::string_view aText, bool aIsError)
{
    // CFUserNotification works from any thread and before NSApplication exists (we run before main)
    auto message = CFStringCreateWithBytes(nullptr, reinterpret_cast<const UInt8*>(aText.data()),
                                           static_cast<CFIndex>(aText.size()), kCFStringEncodingUTF8, false);

    CFOptionFlags response;
    CFUserNotificationDisplayAlert(0, aIsError ? kCFUserNotificationStopAlertLevel : kCFUserNotificationCautionAlertLevel,
                                   nullptr, nullptr, nullptr, CFSTR("TWASE"), message, nullptr, nullptr, nullptr,
                                   &response);

    if (message)
    {
        CFRelease(message);
    }
}

void Utils::Exit()
{
    spdlog::shutdown();
    _exit(1);
}

bool Utils::IsDebuggerPresent()
{
    kinfo_proc info{};
    size_t size = sizeof(info);
    int mib[] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()};

    if (sysctl(mib, 4, &info, &size, nullptr, 0) != 0)
    {
        return false;
    }

    return (info.kp_proc.p_flag & P_TRACED) != 0;
}
