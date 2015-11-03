#include <picotorrent/app/controllers/torrent_context_menu_controller.hpp>

#include <picotorrent/app/controllers/move_torrent_controller.hpp>
#include <picotorrent/app/controllers/remove_torrent_controller.hpp>
#include <picotorrent/core/hash.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/ui/main_window.hpp>
#include <picotorrent/ui/resources.hpp>
#include <picotorrent/ui/torrent_context_menu.hpp>

using picotorrent::core::session;
using picotorrent::core::torrent;
using picotorrent::ui::main_window;
using picotorrent::ui::torrent_context_menu;

using picotorrent::app::controllers::move_torrent_controller;
using picotorrent::app::controllers::remove_torrent_controller;
using picotorrent::app::controllers::torrent_context_menu_controller;

torrent_context_menu_controller::torrent_context_menu_controller(
    const std::shared_ptr<session> &session,
    const std::shared_ptr<torrent> &torrent,
    const std::shared_ptr<main_window> &wnd)
    : sess_(session),
    torrent_(torrent),
    wnd_(wnd)
{
}

void torrent_context_menu_controller::execute(const POINT &p)
{
    torrent_context_menu menu;

    if (torrent_->is_paused())
    {
        menu.remove_pause();
        menu.highlight_resume();
    }
    else
    {
        menu.remove_resume();
        menu.highlight_pause();
    }

    switch (menu.show(wnd_->handle(), p))
    {
    case TORRENT_CONTEXT_MENU_PAUSE:
        torrent_->pause();
        break;

    case TORRENT_CONTEXT_MENU_RESUME:
        torrent_->resume();
        break;

    case TORRENT_CONTEXT_MENU_MOVE:
    {
        move_torrent_controller move_controller(wnd_, torrent_);
        move_controller.execute();
        break;
    }

    case TORRENT_CONTEXT_MENU_REMOVE:
    {
        remove_torrent_controller remove_controller(wnd_, sess_, torrent_);
        remove_controller.execute();
        break;
    }

    case TORRENT_CONTEXT_MENU_COPY_MAGNET:
    {
        break;
    }

    case TORRENT_CONTEXT_MENU_COPY_SHA:
    {
        copy_to_clipboard(torrent_->info_hash()->to_string());
        break;
    }
    }
}

void torrent_context_menu_controller::copy_to_clipboard(const std::string &text)
{
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    memcpy(GlobalLock(hMem), text.c_str(), text.size());
    GlobalUnlock(hMem);

    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
}
