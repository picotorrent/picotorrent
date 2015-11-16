#pragma once

#include <functional>
#include <string>
#include <windows.h>

namespace picotorrent
{
namespace ui
{
    class preferences_dialog
    {
    public:
        preferences_dialog(HWND hParent);

        int do_modal();
        bool get_checked(int controlId);
        std::wstring get_text(int controlId);
        void on_ok(const std::function<void(preferences_dialog&)> &callback);
        void on_init(const std::function<void(preferences_dialog&)> &callback);
        void set_checked(int controlId, bool checked);
        void set_text(int controlId, const std::wstring &text);

    private:
        INT_PTR CALLBACK dlg_proc(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
        static INT_PTR CALLBACK dlg_proc_proxy(_In_ HWND hwndDlg, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

        std::function<void(preferences_dialog&)> ok_cb_;
        std::function<void(preferences_dialog&)> init_cb_;

        HWND hWnd_;
        HWND hParent_;
    };
}
}
