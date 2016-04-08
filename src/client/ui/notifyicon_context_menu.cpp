#include <picotorrent/client/ui/notifyicon_context_menu.hpp>

#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/client/i18n/translator.hpp>
#include <picotorrent/client/ui/resources.hpp>

using picotorrent::client::ui::notifyicon_context_menu;

notifyicon_context_menu::notifyicon_context_menu()
{
    menu_ = CreatePopupMenu();
    AppendMenu(menu_, MF_STRING, ID_NOTIFYICON_ADD, to_wstring(TR("add_torrent")).c_str());
    AppendMenu(menu_, MF_STRING, ID_NOTIFYICON_ADD_MAGNET, to_wstring(TR("add_magnet_link_s")).c_str());
    AppendMenu(menu_, MF_SEPARATOR, 0, NULL);
    AppendMenu(menu_, MF_STRING, ID_NOTIFYICON_EXIT, to_wstring(TR("exit")).c_str());
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
