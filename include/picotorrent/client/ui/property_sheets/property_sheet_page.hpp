#pragma once

#include <picotorrent/core/signals/signal.hpp>

#include <functional>
#include <memory>
#include <windows.h>
#include <prsht.h>

namespace picotorrent
{
namespace client
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
        
        core::signals::signal_connector<void, void>& on_activate();
        core::signals::signal_connector<void, void>& on_apply();
        core::signals::signal_connector<void, void>& on_destroy();
        core::signals::signal_connector<void, void>& on_init();
        core::signals::signal_connector<bool, void>& on_validate();

        HWND handle();
        void show_error_message(const std::wstring &text);

    protected:
        INT_PTR dlg_proc(HWND, UINT, WPARAM, LPARAM);
        bool is_initializing();
        virtual BOOL on_command(HWND hDlg, UINT uCtrlId, WPARAM wParam, LPARAM lParam) { return FALSE; }
        virtual void on_init_dialog() { }
        virtual bool on_notify(HWND hDlg, LPNMHDR nmhdr, LRESULT &result) { return false; }

        std::wstring get_dlg_item_text(int id);
        void set_dlg_item_text(int id, const std::wstring &text);

        void set_flags(DWORD flags);
        void set_instance(HINSTANCE instance);
        void set_template_id(int id);
        void set_title(const std::wstring &title);

        static INT_PTR CALLBACK dlg_proc_proxy(
            _In_ HWND   hwndDlg,
            _In_ UINT   uMsg,
            _In_ WPARAM wParam,
            _In_ LPARAM lParam
            );

    private:
        bool is_initializing_;

        HWND handle_;
        std::wstring title_;
        std::unique_ptr<PROPSHEETPAGE> page_;

        // Signals
        core::signals::signal<void, void> on_activate_;
        core::signals::signal<void, void> on_apply_;
        core::signals::signal<void, void> on_destroy_;
        core::signals::signal<void, void> on_init_;
        core::signals::signal<bool, void> on_validate_;
    };
}
}
}
}
