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
        int get_column_count();

    private:
        HWND header_;
    };
}
}
}
