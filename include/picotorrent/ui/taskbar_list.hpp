#pragma once

#include <stdint.h>
#include <windows.h>
#include <shobjidl.h>

namespace picotorrent
{
namespace ui
{
    class taskbar_list
    {
    public:
        taskbar_list(HWND hParent);
        ~taskbar_list();

        void set_progress_value(uint64_t completed, uint64_t total);

    private:
        ITaskbarList4 *list_;
        HWND hParent_;
    };
}
}