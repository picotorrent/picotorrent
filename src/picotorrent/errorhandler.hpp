#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "../breakpad/exception_handler.h"
#include "../breakpad/crash_report_sender.h"

namespace pt
{
    class ErrorHandler
    {
    public:
        static std::wstring GetDumpPath();

        static bool Filter(void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion);
        static bool Report(const wchar_t* dump_path, const wchar_t* minidump_id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded);
    };
}