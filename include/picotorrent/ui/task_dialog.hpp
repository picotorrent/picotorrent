#pragma once

#include <windows.h>
#include <commctrl.h>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace picotorrent
{
namespace ui
{
    class task_dialog
    {
    public:
        ~task_dialog();

        void add_button(int id, const std::wstring &text, const std::function<bool()> &callback);
        void set_content(const std::wstring &text);
        void set_main_icon(PCWSTR icon);
        void set_main_instruction(const std::wstring& text);
        void set_parent(HWND parent);
        void set_title(const std::wstring &title);
        void show();

    private:
        static HRESULT CALLBACK callback(HWND hWnd, UINT uNotification, WPARAM wParam, LPARAM lParam, LONG_PTR dwRefData);

        std::vector<TASKDIALOG_BUTTON> buttons_;
        std::map<int, std::function<bool()>> callbacks_;
        std::wstring content_;
        PCWSTR icon_;
        std::wstring instruction_;
        HWND parent_;
        std::wstring title_;
    };
}
}
