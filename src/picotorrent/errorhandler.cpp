#include "errorhandler.hpp"

#include <Windows.h>
#include <CommCtrl.h>

#include <filesystem>

#include "buildinfo.hpp"

namespace fs = std::experimental::filesystem::v1;
using pt::ErrorHandler;

std::wstring ErrorHandler::dumpPath()
{
    TCHAR p[MAX_PATH];
    GetTempPath(ARRAYSIZE(p), p);
    return p;
}

bool ErrorHandler::filter(void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion)
{
    const TASKDIALOG_BUTTON buttons[] =
    {
        { IDOK, L"Send error report\nThe error report is anonymous." }
    };

    TASKDIALOGCONFIG tdc = { 0 };
    tdc.cbSize = sizeof(TASKDIALOGCONFIG);
    tdc.dwFlags = TDF_USE_COMMAND_LINKS;
    tdc.dwCommonButtons = TDCBF_CLOSE_BUTTON;
    tdc.pszWindowTitle = TEXT("PicoTorrent");
    tdc.pszMainIcon = TD_ERROR_ICON;
    tdc.pszMainInstruction = L"PicoTorrent crashed";
    tdc.pszContent = L"PicoTorrent has unfortunately crashed. You can send an error report to aid in improving PicoTorrent.";
    tdc.pButtons = buttons;
    tdc.cButtons = ARRAYSIZE(buttons);

    int btn = 0;

    TaskDialogIndirect(
        &tdc,
        &btn,
        nullptr,
        nullptr);

    switch (btn)
    {
    case IDOK:
        return true;
    }

    return false;
}

bool ErrorHandler::report(const wchar_t* path, const wchar_t* id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded)
{
    fs::path checkpointFile = fs::path(dumpPath()) / fs::path("PicoTorrent.checkpoint");

    google_breakpad::CrashReportSender sender(checkpointFile.wstring());
    sender.set_max_reports_per_day(5);

    std::map<std::wstring, std::wstring> params;
    std::map<std::wstring, std::wstring> files;

    QString branch = BuildInfo::branch();
    QString commitish = BuildInfo::commitish();
    QString version = BuildInfo::version();

    params.insert({ L"branch", branch.toStdWString() });
    params.insert({ L"commitish", commitish.toStdWString() });
    params.insert({ L"version", version.toStdWString() });

    fs::path dmpFile = fs::path(path) / fs::path(id).replace_extension(".dmp");
    files.insert({ L"minidump", dmpFile.wstring() });

    sender.SendCrashReport(
        L"https://api.picotorrent.org/minidump",
        params,
        files,
        nullptr);

    return true;
}
