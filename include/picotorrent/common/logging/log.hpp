#pragma once

#include <functional>
#include <memory>
#include <ostream>
#include <picotorrent/export.hpp>
#include <picotorrent/common/logging/log_record.hpp>
#include <string>
#include <windows.h>

#define LOG(level) \
    ::picotorrent::common::logging::log::instance().open_record(picotorrent::common::logging::log_level::level, __FUNCTION__).stream()

#define LOG_2(level, source) \
    ::picotorrent::common::logging::log::instance().open_record(picotorrent::common::logging::log_level::level, source).stream()

namespace picotorrent
{
namespace common
{
namespace logging
{
    enum log_level
    {
        trace,
        debug,
        info,
        warning,
        error,
        fatal
    };

    class log
    {
    public:
        log();
        ~log();

        DLL_EXPORT static log& instance();

        DLL_EXPORT log_record open_record(log_level level, const char* functionName);
        DLL_EXPORT void set_unhandled_exception_callback(const std::function<void(const std::string&)> &callback);

    private:
        void init();

        static LONG WINAPI on_unhandled_exception(PEXCEPTION_POINTERS exceptionInfo);

        std::function<void(const std::string&)> unhandled_exception_callback_;
        std::unique_ptr<std::ostream> out_;
    };

    std::ostream& operator<<(std::ostream &stream, const log_level level);
}
}
}
