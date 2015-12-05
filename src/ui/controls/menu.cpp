#include <picotorrent/ui/controls/menu.hpp>

using picotorrent::ui::controls::menu;

menu::menu(int resourceId)
    : menu_(NULL)
{
    menu_ = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId));
}

menu::menu(HMENU handle)
    : menu_(handle)
{
}

menu::~menu()
{
    DestroyMenu(menu_);
}

void menu::check_item(int id)
{
    CheckMenuItem(menu_, id, MF_BYCOMMAND | MF_CHECKED);
}

menu menu::get_sub_menu(int index)
{
    return menu(GetSubMenu(menu_, index));
}

HMENU menu::handle()
{
    return menu_;
}

int menu::show(HWND parent, const POINT &point)
{
    return TrackPopupMenu(
        menu_,
        TPM_NONOTIFY | TPM_RETURNCMD,
        point.x,
        point.y,
        0,
        parent,
        NULL);
}
