#pragma once

#include <functional>
#include <memory>
#include <ostream>
#include <picotorrent/common.hpp>
#include <picotorrent/core/logging/log_record.hpp>
#include <string>
#include <windows.h>

#define LOG(level) \
    ::picotorrent::core::logging::log::instance().open_record(picotorrent::core::logging::log_level::level, __FUNCTION__).stream()

namespace picotorrent
{
namespace core
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
        DLL_EXPORT log();
        DLL_EXPORT ~log();

        DLL_EXPORT static log& instance();

        DLL_EXPORT void init();
        DLL_EXPORT log_record open_record(log_level level, const char* functionName);
        DLL_EXPORT void set_unhandled_exception_callback(const std::function<void(const std::string&)> &callback);

    private:
        static LONG WINAPI on_unhandled_exception(PEXCEPTION_POINTERS exceptionInfo);

        std::function<void(const std::string&)> unhandled_exception_callback_;
        std::unique_ptr<std::ostream> out_;
    };

    std::ostream& operator<<(std::ostream &stream, const log_level level);
}
}
}
