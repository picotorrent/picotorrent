#pragma once

#include <windows.h>

namespace picotorrent
{
namespace client
{
namespace ui
{
namespace controls
{
    class menu
    {
    public:
        menu(int resourceId);
        menu(HMENU handle);
        ~menu();

        void check_item(int id);
        menu get_sub_menu(int index);
        HMENU handle();
        int show(HWND parent, const POINT &point);

    private:
        HMENU menu_;
    };
}
}
}
}
