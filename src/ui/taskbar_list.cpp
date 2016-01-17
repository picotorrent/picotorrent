#include <picotorrent/ui/taskbar_list.hpp>

using picotorrent::ui::taskbar_list;

taskbar_list::taskbar_list(HWND hParent)
    : hParent_(hParent)
{
    CoInitialize(NULL);
    CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList4, (void **)&list_);
}

taskbar_list::~taskbar_list()
{
    if (list_)
    {
        list_->Release();
        CoUninitialize();
    }
}

void taskbar_list::set_progress_state(TBPFLAG flags)
{
    list_->SetProgressState(hParent_, flags);
}

void taskbar_list::set_progress_value(uint64_t completed, uint64_t total)
{
    list_->SetProgressValue(hParent_, completed, total);
}
