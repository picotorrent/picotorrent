#include "application.h"
#include "mainwindow.h"
#include "stdafx.h"

using pico::Application;

bool Application::Initialize()
{
    // Initialize common controls
    INITCOMMONCONTROLSEX icex = { 0 };
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);

    if (!InitCommonControlsEx(&icex))
    {
        DWORD err = GetLastError();
        return false;
    }

    return true;
}

int Application::Run()
{
    MainWindow win;
    win.Create();

    MSG msg;
    BOOL ret;

    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (ret == -1)
        {
            // TODO(handle error)
        }
        else
        {
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
}
