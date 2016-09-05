#include "Taskbar.hpp"

using UI::Taskbar;

Taskbar::Taskbar(HWND hWndParent)
    : m_hWndParent(hWndParent)
{
    CoInitialize(NULL);
    CoCreateInstance(
        CLSID_TaskbarList,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_ITaskbarList4,
        (void **)&m_taskbarList);
}

Taskbar::~Taskbar()
{
    m_taskbarList->Release();
    CoUninitialize();
}


void Taskbar::SetProgressState(TBPFLAG flags)
{
    m_taskbarList->SetProgressState(m_hWndParent, flags);
}

void Taskbar::SetProgressValue(uint64_t completed, uint64_t total)
{
    m_taskbarList->SetProgressValue(m_hWndParent, completed, total);
}
