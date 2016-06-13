#pragma once

#include <windows.h>

namespace PicoTorrent
{
namespace UI
{
    ref class MainWindow : public IMainWindow
    {
    public:
        MainWindow(HWND hWnd);

        virtual void ShowMessageBox(System::String^ title, System::String^ message);

    private:
        HWND _hWnd;
    };
}
}
