#pragma once

#include <string>
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
        void show_balloon(const std::wstring &title, const std::wstring &text);

    private:
        HWND parent_;
        HICON icon_;
    };
}
}
