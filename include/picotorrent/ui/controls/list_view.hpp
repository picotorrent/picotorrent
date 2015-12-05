#pragma once

#include <picotorrent/ui/controls/control_base.hpp>
#include <string>

namespace picotorrent
{
namespace ui
{
namespace controls
{
    class list_view : public control_base
    {
    public:
        list_view(HWND handle);
        void add_column(const std::wstring &text, int width, int format);
        void clear();
        int get_column_count();
        int get_item_count();
        int get_selected_index();
        void insert_item(int index, const std::wstring &text);
        void set_extended_style(int style);
        void set_item(int index, int sub_index, const std::wstring &text);

    private:
        HWND header_;
    };
}
}
}
