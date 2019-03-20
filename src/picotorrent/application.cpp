#include "application.hpp"

#include <Windows.h>

#include "picojson.hpp"

using pt::Application;

struct Application::Mutex
{
    HANDLE hndl;
};

Application::Application(int& argc, char **argv)
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
    picojson::array arr;

    for (QString const& arg : QApplication::arguments())
    {
        arr.push_back(picojson::value(arg.toStdString()));
    }

    picojson::value val(arr);
    std::string encodedArgs = val.serialize();

    COPYDATASTRUCT cds;
    cds.cbData = encodedArgs.size();
    cds.dwData = 1;
    cds.lpData = reinterpret_cast<PVOID>(&encodedArgs[0]);

    // Activate other window
    HWND hWndOther = FindWindow(NULL, TEXT("PicoTorrent"));

    SetForegroundWindow(hWndOther);
    ShowWindow(hWndOther, SW_RESTORE);
    SendMessage(hWndOther, WM_COPYDATA, NULL, (LPARAM)&cds);
}

bool Application::isSingleInstance()
{
    return m_isSingleInstance;
}
