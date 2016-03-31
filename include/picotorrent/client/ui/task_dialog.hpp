#pragma once

#include <windows.h>
#include <commctrl.h>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace picotorrent
{
namespace client
{
namespace ui
{
    class task_dialog
    {
    public:
        task_dialog();
        ~task_dialog();

        void add_button(int id, const std::string &text, const std::function<bool()> &callback);
        bool is_verification_checked();
        void set_common_buttons(DWORD buttons);
        void set_content(const std::string &text);
        void set_main_icon(PCWSTR icon);
        void set_main_instruction(const std::string& text);
        void set_parent(HWND parent);
        void set_title(const std::string &title);
        void set_verification_text(const std::string &text);
        int show();

    private:
        static HRESULT CALLBACK callback(HWND hWnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData);

        std::vector<TASKDIALOG_BUTTON> buttons_;
        std::map<int, std::function<bool()>> callbacks_;
        DWORD common_buttons_;
        std::wstring content_;
        PCWSTR icon_;
        std::wstring instruction_;
        HWND parent_;
        std::wstring title_;
        std::wstring verification_;
        bool verification_checked_;
    };
}
}
}
