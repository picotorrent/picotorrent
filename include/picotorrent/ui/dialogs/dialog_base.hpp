#pragma once

#include <windows.h>

namespace picotorrent
{
namespace ui
{
namespace dialogs
{
    class dialog_base
    {
    public:
        dialog_base(int id);
        ~dialog_base();

        int show_modal(HWND parent);

    protected:
        HWND handle();
        virtual BOOL on_command(int id, WPARAM, LPARAM);
        virtual BOOL on_init_dialog();
        virtual BOOL on_notify(LPARAM);

    private:
        virtual INT_PTR CALLBACK dlg_proc(HWND, UINT, WPARAM, LPARAM);
        static INT_PTR CALLBACK dlg_proc_proxy(HWND, UINT, WPARAM, LPARAM);

        int id_;
        HWND handle_;
    };
}
}
}