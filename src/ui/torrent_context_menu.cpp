#include <picotorrent/ui/torrent_context_menu.hpp>

#include <picotorrent/i18n/translator.hpp>
#include <picotorrent/ui/resources.hpp>

using picotorrent::ui::torrent_context_menu;

torrent_context_menu::torrent_context_menu()
{
    menu_ = CreatePopupMenu();
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_PAUSE, TR("pause"));
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_RESUME, TR("resume"));
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_RESUME_FORCE, TR("resume_force"));
    AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_DETAILS, TR("details"));
    AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_MOVE, TR("move"));

    HMENU remove = CreateMenu();
    AppendMenu(remove, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE, TR("remove_torrent"));
    AppendMenu(remove, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE_DATA, TR("remove_torrent_and_files"));

    AppendMenu(menu_, MF_POPUP, (UINT_PTR)remove, TR("remove"));
    AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_COPY_SHA, TR("copy_info_hash"));
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, TR("open_in_explorer"));
}

torrent_context_menu::~torrent_context_menu()
{
    DestroyMenu(menu_);
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
