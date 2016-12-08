#include "UnhandledExceptionHandler.hpp"

#include <signal.h>
#include <windows.h>
#include <commctrl.h>
#pragma warning( push )
#pragma warning( disable : 4091 )
#include <dbghelp.h>
#pragma warning ( pop )
#include <shellapi.h>
#include <shlwapi.h>

#include <exception>
#include <iostream>
#include <sstream>

#include "Diagnostics/StackFrame.hpp"
#include "Diagnostics/StackTrace.hpp"
#include "StringUtils.hpp"

#define BTN_SHOWDIAGNOSTICFILES 1002

void UnhandledExceptionHandler::Setup()
{
    // Set the silent treatment and allow our awesome error report dialog
    // to take over.
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

    // Catch signals and unhandled exceptions
    signal(SIGABRT, UnhandledExceptionHandler::OnSignal);
    signal(SIGINT, UnhandledExceptionHandler::OnSignal);
    signal(SIGSEGV, UnhandledExceptionHandler::OnSignal);
    SetUnhandledExceptionFilter(UnhandledExceptionHandler::OnUnhandledException);
}

UnhandledExceptionHandler::UnhandledExceptionHandler(const Diagnostics::StackTrace& stackTrace)
    : m_stackTrace(std::make_unique<Diagnostics::StackTrace>(stackTrace)),
    m_exceptionPointers(NULL)
{
}

UnhandledExceptionHandler::UnhandledExceptionHandler(const Diagnostics::StackTrace& stackTrace, PEXCEPTION_POINTERS exceptionPointers)
    : m_stackTrace(std::make_unique<Diagnostics::StackTrace>(stackTrace)),
    m_exceptionPointers(exceptionPointers)
{
}


HRESULT UnhandledExceptionHandler::DialogCallback(HWND hWnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData)
{
    switch (uNotification)
    {
    case TDN_BUTTON_CLICKED:
    {
        switch (wParam)
        {
        case BTN_SHOWDIAGNOSTICFILES:
        {
            TCHAR tmp[MAX_PATH];
            GetTempPath(ARRAYSIZE(tmp), tmp);
            PathCombine(tmp, tmp, TEXT("PicoTorrentDump"));
            ShellExecute(NULL, TEXT("open"), tmp, NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        }
        break;
    }
    case TDN_HYPERLINK_CLICKED:
    {
        PCTSTR href = reinterpret_cast<PCTSTR>(lParam);
        ShellExecute(NULL, TEXT("open"), href, NULL, NULL, SW_SHOWNORMAL);
        break;
    }
    }

    return S_OK;
}

void UnhandledExceptionHandler::OnSignal(int signal)
{
    switch (signal)
    {
    case SIGABRT:
    case SIGSEGV:
    {
        Diagnostics::StackTrace stackTrace = Diagnostics::StackTrace::Capture();

        UnhandledExceptionHandler ueh(stackTrace);
        ueh.GenerateDiagnosticFiles();
        ueh.ShowDialog();
    };
    }
}

LONG UnhandledExceptionHandler::OnUnhandledException(PEXCEPTION_POINTERS pExceptionPtrs)
{
    Diagnostics::StackTrace stackTrace = Diagnostics::StackTrace::Capture();

    UnhandledExceptionHandler ueh(stackTrace, pExceptionPtrs);
    ueh.GenerateDiagnosticFiles();
    ueh.ShowDialog();

    return EXCEPTION_EXECUTE_HANDLER;
}

void UnhandledExceptionHandler::GenerateDiagnosticFiles()
{
    // Get a temporary path
    TCHAR tmp[MAX_PATH];
    GetTempPath(ARRAYSIZE(tmp), tmp);
    PathCombine(tmp, tmp, TEXT("PicoTorrentDump"));
    CreateDirectory(tmp, NULL);

    TCHAR dump[MAX_PATH];
    PathCombine(dump, tmp, TEXT("minidump.dmp"));

    TCHAR trace[MAX_PATH];
    PathCombine(trace, tmp, TEXT("stacktrace.log"));

    HANDLE hFile = CreateFile(
        dump,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = m_exceptionPointers;
        mdei.ClientPointers = FALSE;

        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            MiniDumpNormal,
            (m_exceptionPointers != 0) ? &mdei : 0,
            0,
            0);

        CloseHandle(hFile);
    }

    hFile = CreateFile(
        trace,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile != NULL && hFile != INVALID_HANDLE_VALUE)
    {
        std::stringstream ss;
        for (auto& frame : m_stackTrace->frames)
        {
            ss << frame.address << " : " << frame.name << " (" << frame.fileName << " line " << frame.lineNumber << ")" << std::endl;
        }

        std::string t = ss.str();
        WriteFile(hFile, &t[0], (DWORD)t.size(), NULL, NULL);

        CloseHandle(hFile);
    }
}

void UnhandledExceptionHandler::ShowDialog()
{
    const TASKDIALOG_BUTTON pButtons[] =
    {
        { BTN_SHOWDIAGNOSTICFILES, TEXT("Show diagnostic files\nPlease include these in your error report.") }
    };

    std::stringstream ss;
    for (auto& frame : m_stackTrace->frames)
    {
        ss << frame.name << ":" << frame.lineNumber << std::endl;
    }

    TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
    tdf.cButtons = ARRAYSIZE(pButtons);
    tdf.dwCommonButtons = TDCBF_CLOSE_BUTTON;
    tdf.dwFlags = TDF_ENABLE_HYPERLINKS | TDF_USE_COMMAND_LINKS;
    tdf.hwndParent = NULL;
    tdf.lpCallbackData = reinterpret_cast<LONG_PTR>(this);
    tdf.pButtons = pButtons;
    tdf.pfCallback = &UnhandledExceptionHandler::DialogCallback;
    tdf.pszCollapsedControlText = TEXT("Show stack trace");
    tdf.pszContent = TEXT("You can help the development of PicoTorrent by <a href=\"https://github.com/picotorrent/picotorrent/issues\">opening an issue on GitHub</a> or by submitting the (anonymous) error report.");
    tdf.pszExpandedControlText = TEXT("Hide stack trace");

    std::wstring expanded = TWS(ss.str());
    tdf.pszExpandedInformation = expanded.c_str();

    tdf.pszMainIcon = TD_ERROR_ICON;
    tdf.pszMainInstruction = TEXT("Unhandled exception occured");
    tdf.pszWindowTitle = TEXT("PicoTorrent");

    TaskDialogIndirect(&tdf, NULL, NULL, NULL);
}
