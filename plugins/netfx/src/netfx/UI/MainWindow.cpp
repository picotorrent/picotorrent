#include "MainWindow.h"

#include "MainMenu.h"
#include "TaskDialog.h"

#include <msclr/marshal_cppstd.h>

using PicoTorrent::UI::IMainMenu;
using PicoTorrent::UI::MainMenu;
using PicoTorrent::UI::MainWindow;
using PicoTorrent::UI::MainWindowWrapper;

class MainWindowWrapper
{
public:
    MainWindowWrapper(gcroot<PicoTorrent::UI::MainWindow^> window)
        : _window(window)
    {
    }

    gcroot<PicoTorrent::UI::MainWindow^> get() { return _window; }

private:
    gcroot<PicoTorrent::UI::MainWindow^> _window;
};

LRESULT CALLBACK MainWindowSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    MainWindowWrapper* wrapper = reinterpret_cast<MainWindowWrapper*>(dwRefData);

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        wrapper->get()->RaiseCommand(gcnew PicoTorrent::UI::CommandEventArgs(LOWORD(wParam)));
        break;
    }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

MainWindow::MainWindow(HWND hWnd)
    : _hWnd(hWnd),
    _wrapper(new MainWindowWrapper(this))
{
    SetWindowSubclass(
        hWnd,
        &MainWindowSubclassProc,
        1337,
        (DWORD_PTR)_wrapper);
}

PicoTorrent::UI::ITaskDialog^ MainWindow::CreateTaskDialog()
{
    return gcnew UI::TaskDialog(_hWnd);
}

IMainMenu^ MainWindow::MainMenu::get()
{
    HMENU hm = GetMenu(_hWnd);
    return gcnew PicoTorrent::UI::MainMenu(hm);
}

void MainWindow::ShowMessageBox(System::String^ title, System::String^ message)
{
    msclr::interop::marshal_context^ context = gcnew msclr::interop::marshal_context();

    MessageBox(
        _hWnd,
        context->marshal_as<LPCTSTR>(message),
        context->marshal_as<LPCTSTR>(title),
        MB_OK);
}

void MainWindow::RaiseCommand(PicoTorrent::UI::CommandEventArgs^ args)
{
    Command(this, args);
}
