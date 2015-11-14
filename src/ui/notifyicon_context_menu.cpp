#include <picotorrent/ui/notifyicon_context_menu.hpp>

#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::notifyicon_context_menu;

notifyicon_context_menu::notifyicon_context_menu()
{
    HMENU ctxMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_NOTIFYICON_CONTEXT_MENU));
    menu_ = GetSubMenu(ctxMenu, 0);
}

notifyicon_context_menu::~notifyicon_context_menu()
{
    DestroyMenu(menu_);
}

int notifyicon_context_menu::show(HWND hWnd, const POINT &p)
{
    return TrackPopupMenu(
        menu_,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        hWnd,
        NULL);
}
