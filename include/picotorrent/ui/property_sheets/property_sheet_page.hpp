#pragma once

#include <functional>
#include <memory>
#include <windows.h>
#include <prsht.h>

namespace picotorrent
{
namespace ui
{
namespace property_sheets
{
    class property_sheet_page
    {
    public:
        property_sheet_page();

        operator PROPSHEETPAGE&()
        {
            return *page_.get();
        }
        
        void set_init_callback(const std::function<void()> &callback);

    protected:
        INT_PTR dlg_proc(HWND, UINT, WPARAM, LPARAM);
        HWND handle();
        void set_flags(DWORD flags);
        void set_instance(HINSTANCE instance);
        void set_template_id(int id);
        void set_title_id(int id);

        static INT_PTR CALLBACK dlg_proc_proxy(
            _In_ HWND   hwndDlg,
            _In_ UINT   uMsg,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam
            );

    private:
        HWND handle_;
        std::unique_ptr<PROPSHEETPAGE> page_;
        std::function<void()> init_cb_;
    };
}
}
}
