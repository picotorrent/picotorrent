#pragma once

#include <picotorrent/common/signals/signal.hpp>
#include <picotorrent/ui/controls/control_base.hpp>
#include <string>
#include <vector>

#include <commctrl.h>

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
        static void register_window();

        void add_column(int id, const std::wstring &text, int width, col_type_t type = col_type_t::text);
        common::signals::signal_connector<std::wstring, int>& on_display();



        void clear();
        int get_column_count();
        int get_item_count();
        std::vector<int> get_selected_items();
        void insert_item(int index, const std::wstring &text);
        void set_extended_style(int style);
        void set_item(int index, int sub_index, const std::wstring &text);

    private:
        static LRESULT CALLBACK subclass_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

        struct list_view_column
        {
            int id;
            int index;
            std::wstring text;
            int width;
            int format;
            col_type_t type;

            LVCOLUMN get_native_column();
        };

        HWND header_;
        std::vector<list_view_column> columns_;
        common::signals::signal<std::wstring, int> on_display_;
    };
}
}
}
