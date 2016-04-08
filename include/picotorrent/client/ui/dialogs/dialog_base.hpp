#pragma once

#include <string>
#include <windows.h>

namespace picotorrent
{
namespace client
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

        void close();
        HWND handle();
        int show_modal(HWND parent);

    protected:
        std::string get_dlg_item_text(int controlId);
        bool is_dlg_button_checked(int controlId);
        void set_dlg_button_checked(int controlId, bool checked);
        void set_dlg_item_text(int controlId, const std::string &text);
        void set_window_text(const std::string &text);

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
}
