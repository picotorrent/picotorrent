#pragma once

#include "../breakpad/exception_handler.h"
#include "../breakpad/crash_report_sender.h"

namespace pt
{
    class ErrorHandler
    {
    public:
        static std::wstring dumpPath();

        static bool filter(void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion);
        static bool report(const wchar_t* dump_path, const wchar_t* minidump_id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded);
    };
}
