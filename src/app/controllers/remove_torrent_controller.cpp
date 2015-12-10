#include <picotorrent/app/controllers/remove_torrent_controller.hpp>

#include <algorithm>
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
    const std::vector<std::shared_ptr<torrent>> &torrents)
    : wnd_(wnd),
    session_(session),
    torrents_(torrents)
{
}

remove_torrent_controller::~remove_torrent_controller()
{
}

void remove_torrent_controller::execute()
{
    if (torrents_.empty())
    {
        return;
    }

    remove_torrent_dialog dlg;

    switch (dlg.show(wnd_->handle()))
    {
    case ID_REMOVE_DATA:
    {
        for (const std::shared_ptr<torrent> &t : torrents_)
        {
            session_->remove_torrent(t, true);
        }
        break;
    }

    case ID_KEEP_DATA:
    {
        for (const std::shared_ptr<torrent> &t : torrents_)
        {
            session_->remove_torrent(t, false);
        }
        break;
    }
    }
}
