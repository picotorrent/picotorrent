#include <picotorrent/ui/torrent_context_menu.hpp>

#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::torrent_context_menu;

torrent_context_menu::torrent_context_menu()
{
    menu_ = LoadMenu(NULL, MAKEINTRESOURCE(IDR_TORRENT_CONTEXT_MENU));
    menu_ = GetSubMenu(menu_, 0);
}

void torrent_context_menu::disable_open_in_explorer()
{
    EnableMenuItem(menu_, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, MF_BYCOMMAND | MF_DISABLED);
}

void torrent_context_menu::highlight_pause()
{
    SetMenuDefaultItem(menu_, TORRENT_CONTEXT_MENU_PAUSE, FALSE);
}

void torrent_context_menu::highlight_resume()
{
    SetMenuDefaultItem(menu_, TORRENT_CONTEXT_MENU_RESUME, FALSE);
}

void torrent_context_menu::remove_pause()
{
    RemoveMenu(menu_, TORRENT_CONTEXT_MENU_PAUSE, MF_BYCOMMAND);
}

void torrent_context_menu::remove_resume()
{
    RemoveMenu(menu_, TORRENT_CONTEXT_MENU_RESUME, MF_BYCOMMAND);
    RemoveMenu(menu_, TORRENT_CONTEXT_MENU_RESUME_FORCE, MF_BYCOMMAND);
}

int torrent_context_menu::show(HWND parent, const POINT &p)
{
    return TrackPopupMenu(
        menu_,
        TPM_NONOTIFY | TPM_RETURNCMD,
        p.x,
        p.y,
        0,
        parent,
        NULL);
}
