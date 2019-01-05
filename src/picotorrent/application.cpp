#include "application.hpp"

#include <Windows.h>

using pt::Application;

struct Application::Mutex
{
    HANDLE hndl;
};

Application::Application(int argc, char **argv)
    : QApplication(argc, argv),
    m_singleInstanceMutex(std::make_unique<Mutex>())
{
    m_singleInstanceMutex->hndl = CreateMutex(NULL, FALSE, TEXT("PicoTorrent/1.0"));
    m_isSingleInstance = (GetLastError() != ERROR_ALREADY_EXISTS);
}

Application::~Application()
{
    CloseHandle(m_singleInstanceMutex->hndl);
}

void Application::activateOtherInstance()
{
    HWND hWndOther = FindWindow(NULL, TEXT("PicoTorrent"));
    LPTSTR cmdLine = GetCommandLine();
    std::wstring args = cmdLine;

    COPYDATASTRUCT cds;
    cds.cbData = static_cast<DWORD>(sizeof(wchar_t) * (args.size() + 1));
    cds.dwData = 1;
    cds.lpData = reinterpret_cast<PVOID>(&args[0]);

    // Activate other window
    SetForegroundWindow(hWndOther);
    ShowWindow(hWndOther, SW_RESTORE);
    SendMessage(hWndOther, WM_COPYDATA, NULL, (LPARAM)&cds);
}

bool Application::isSingleInstance()
{
    return m_isSingleInstance;
}
