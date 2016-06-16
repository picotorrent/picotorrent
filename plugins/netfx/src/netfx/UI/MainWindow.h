#pragma once

#include <windows.h>

namespace PicoTorrent
{
namespace UI
{
    class MainWindowWrapper;

    ref class MainWindow : public IMainWindow
    {
    public:
        MainWindow(HWND hWnd);

        virtual event System::EventHandler<CommandEventArgs^>^ Command;

        virtual ITaskDialog^ CreateTaskDialog();

        virtual property IMainMenu^ MainMenu { IMainMenu^ get(); }

        virtual void ShowMessageBox(System::String^ title, System::String^ message);

        void RaiseCommand(CommandEventArgs^);

    private:
        MainWindowWrapper* _wrapper;
        HWND _hWnd;
    };
}
}
