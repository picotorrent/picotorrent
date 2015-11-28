#pragma once

#include <windows.h>

namespace picotorrent
{
namespace ui
{
    class about_dialog
    {
    public:
        about_dialog(HWND parent);
        ~about_dialog();

        int show_modal();

    private:
        INT_PTR CALLBACK dlg_proc(HWND, UINT, WPARAM, LPARAM);
        static INT_PTR CALLBACK dlg_proc_proxy(HWND, UINT, WPARAM, LPARAM);

        HWND handle_;
        HWND parent_;
        HFONT title_font_;
    };
}
}
