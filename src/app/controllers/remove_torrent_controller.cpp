#include <picotorrent/app/controllers/remove_torrent_controller.hpp>

#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/ui/remove_torrent_dialog.hpp>
#include <picotorrent/ui/main_window.hpp>

using picotorrent::app::controllers::remove_torrent_controller;
using picotorrent::core::session;
using picotorrent::core::torrent;
using picotorrent::ui::remove_torrent_dialog;
using picotorrent::ui::main_window;

remove_torrent_controller::remove_torrent_controller(
    const std::shared_ptr<main_window> &wnd,
    const std::shared_ptr<session> &session,
    const std::shared_ptr<torrent> &torrent)
    : wnd_(wnd),
    session_(session),
    torrent_(torrent)
{
}

remove_torrent_controller::~remove_torrent_controller()
{
}

void remove_torrent_controller::execute()
{
    remove_torrent_dialog dlg;

    switch (dlg.show(wnd_->handle()))
    {
    case ID_REMOVE_DATA:
    {
        session_->remove_torrent(torrent_, true);
        break;
    }

    case ID_KEEP_DATA:
    {
        session_->remove_torrent(torrent_, false);
        break;
    }
    }
}
