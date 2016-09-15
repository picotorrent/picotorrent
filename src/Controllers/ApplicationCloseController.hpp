#pragma once

#include <windows.h>

namespace Controllers
{
    class ApplicationCloseController
    {
    public:
        ApplicationCloseController(HWND hWndMainWindow);
        bool Execute();

    private:
        bool Prompt();

        HWND m_hWndMainWindow;
    };
}
