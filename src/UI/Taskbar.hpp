#pragma once

#include <stdint.h>
#include <windows.h>
#include <shobjidl.h>

namespace UI
{
    class Taskbar
    {
    public:
        Taskbar(HWND hWndParent);
        ~Taskbar();

        void SetProgressState(TBPFLAG flags);
        void SetProgressValue(uint64_t completed, uint64_t total);

    private:
        ITaskbarList4* m_taskbarList;
        HWND m_hWndParent;
    };
}
