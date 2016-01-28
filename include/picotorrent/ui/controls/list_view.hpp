#pragma once

#include <picotorrent/common/signals/signal.hpp>
#include <picotorrent/ui/controls/control_base.hpp>
#include <map>
#include <string>

#include <commctrl.h>
#include <uxtheme.h>

namespace picotorrent
{
namespace ui
{
namespace controls
{
    class list_view : public control_base
    {
    public:
        enum col_type_t
        {
            text,
            number,
            progress
        };

        list_view(HWND handle);
        ~list_view();

        void add_column(int id, const std::wstring &text, int width, col_type_t type = col_type_t::text);
        std::vector<int> get_selection();
        
        common::signals::signal_connector<std::wstring, const std::pair<int, int>&>& on_display();
        common::signals::signal_connector<void, const std::vector<int>&>& on_item_context_menu();
        common::signals::signal_connector<float, const std::pair<int, int>&>& on_progress();

        void refresh();
        void set_item_count(int count);

    private:
        static LRESULT CALLBACK subclass_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

        struct list_view_column
        {
            int id;
            std::wstring text;
            int width;
            int format;
            col_type_t type;

            LVCOLUMN get_native_column();
        };

        HWND header_;
        HTHEME progress_theme_;
        HWND progress_;

        std::map<int, list_view_column> columns_;
        common::signals::signal<std::wstring, const std::pair<int, int>&> on_display_;
        common::signals::signal<void, const std::vector<int>&> on_item_context_;
        common::signals::signal<float, const std::pair<int, int>&> on_progress_;
    };
}
}
}
