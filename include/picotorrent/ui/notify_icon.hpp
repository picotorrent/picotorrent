#pragma once

#include <windows.h>

namespace picotorrent
{
namespace ui
{
    class notify_icon
    {
    public:
        notify_icon(HWND parent);
        ~notify_icon();

        void add();
        void remove();

    private:
        HWND parent_;
    };
}
}
