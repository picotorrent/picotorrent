#include <picotorrent/client/ui/torrent_context_menu.hpp>

#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>

using picotorrent::client::ui::torrent_context_menu;

torrent_context_menu::torrent_context_menu()
{
    menu_ = CreatePopupMenu();
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_PAUSE, to_wstring(TR("pause")).c_str());
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_RESUME, to_wstring(TR("resume")).c_str());
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_RESUME_FORCE, to_wstring(TR("resume_force")).c_str());
    AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_DETAILS, to_wstring(TR("details")).c_str());
    AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_MOVE, to_wstring(TR("move")).c_str());

    HMENU queue = CreateMenu();
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_UP, to_wstring(TR("up")).c_str());
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_DOWN, to_wstring(TR("down")).c_str());
    AppendMenu(queue, MF_SEPARATOR, 0, NULL);
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_TOP, to_wstring(TR("top")).c_str());
    AppendMenu(queue, MF_STRING, TORRENT_CONTEXT_MENU_QUEUE_BOTTOM, to_wstring(TR("bottom")).c_str());

    HMENU remove = CreateMenu();
    AppendMenu(remove, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE, to_wstring(TR("remove_torrent")).c_str());
    AppendMenu(remove, MF_STRING, TORRENT_CONTEXT_MENU_REMOVE_DATA, to_wstring(TR("remove_torrent_and_files")).c_str());

    AppendMenu(menu_, MF_POPUP, (UINT_PTR)queue, to_wstring(TR("queuing")).c_str());
    AppendMenu(menu_, MF_POPUP, (UINT_PTR)remove, to_wstring(TR("remove")).c_str());
    AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_COPY_SHA, to_wstring(TR("copy_info_hash")).c_str());
    AppendMenu(menu_, MF_STRING, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, to_wstring(TR("open_in_explorer")).c_str());
}

torrent_context_menu::~torrent_context_menu()
{
    DestroyMenu(menu_);
}

void torrent_context_menu::disable_open_in_explorer()
{
    EnableMenuItem(menu_, TORRENT_CONTEXT_MENU_OPEN_IN_EXPLORER, MF_BYCOMMAND | MF_DISABLED);
}

void torrent_context_menu::disable_queuing()
{
    EnableMenuItem(menu_, TORRENT_CONTEXT_MENU_QUEUE_UP, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(menu_, TORRENT_CONTEXT_MENU_QUEUE_DOWN, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(menu_, TORRENT_CONTEXT_MENU_QUEUE_TOP, MF_BYCOMMAND | MF_DISABLED);
    EnableMenuItem(menu_, TORRENT_CONTEXT_MENU_QUEUE_BOTTOM, MF_BYCOMMAND | MF_DISABLED);
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
