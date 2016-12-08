#pragma once

#include <memory>
#include <string>

#include <windows.h>

namespace Diagnostics
{
    class StackTrace;
}

class UnhandledExceptionHandler
{
public:
    UnhandledExceptionHandler(const Diagnostics::StackTrace& stackTrace);
    UnhandledExceptionHandler(const Diagnostics::StackTrace& stackTrace, PEXCEPTION_POINTERS exceptionPointers);

    static void Setup();

private:
    static HRESULT CALLBACK DialogCallback(
        _In_ HWND     hwnd,
        _In_ UINT     uNotification,
        _In_ WPARAM   wParam,
        _In_ LPARAM   lParam,
        _In_ LONG_PTR dwRefData
        );
    static void OnSignal(int signal);
    static LONG WINAPI OnUnhandledException(PEXCEPTION_POINTERS pExceptionPtrs);

    void GenerateDiagnosticFiles();
    void ShowDialog();

    std::unique_ptr<Diagnostics::StackTrace> m_stackTrace;
    PEXCEPTION_POINTERS m_exceptionPointers;
};
