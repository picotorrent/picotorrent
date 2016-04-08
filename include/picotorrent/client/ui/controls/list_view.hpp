#pragma once

#include <picotorrent/core/signals/signal.hpp>
#include <picotorrent/client/ui/controls/control_base.hpp>
#include <map>
#include <string>

#include <commctrl.h>
#include <uxtheme.h>

namespace picotorrent
{
namespace client
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

        enum sort_order_t
        {
            unknown,
            asc,
            desc
        };

        list_view(HWND handle);
        ~list_view();

        void add_column(int id, const std::string &text, int width, col_type_t type = col_type_t::text);
        int get_column_count();
        std::vector<int> get_selection();
        sort_order_t get_sort_order(int columnId);
        
        core::signals::signal_connector<std::string, const std::pair<int, int>&>& on_display();
        core::signals::signal_connector<void, const std::vector<int>&>& on_item_context_menu();
        core::signals::signal_connector<int, const std::pair<int, int>&>& on_item_image();
        core::signals::signal_connector<float, const std::pair<int, int>&>& on_progress();
        core::signals::signal_connector<void, const std::pair<int, sort_order_t>&>& on_sort();

        void refresh();
        void resize(int width, int height);
        void set_column_sort(int columnId, sort_order_t order);
        void set_image_list(HIMAGELIST img);
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
        core::signals::signal<std::string, const std::pair<int, int>&> on_display_;
        core::signals::signal<void, const std::vector<int>&> on_item_context_;
        core::signals::signal<int, const std::pair<int, int>&> on_item_image_;
        core::signals::signal<float, const std::pair<int, int>&> on_progress_;
        core::signals::signal<void, const std::pair<int, sort_order_t>&> on_sort_;
    };
}
}
}
}
