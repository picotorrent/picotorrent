#include "MainWindow.h"

#include <msclr/marshal_cppstd.h>

using PicoTorrent::UI::MainWindow;

MainWindow::MainWindow(HWND hWnd)
    : _hWnd(hWnd)
{
}

void MainWindow::ShowMessageBox(System::String^ title, System::String^ message)
{
    msclr::interop::marshal_context^ context = gcnew msclr::interop::marshal_context();

    MessageBox(
        _hWnd,
        context->marshal_as<LPCTSTR>(title),
        context->marshal_as<LPCTSTR>(message),
        MB_OK);
}
