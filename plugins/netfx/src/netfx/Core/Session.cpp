#include "Session.h"

#include <functional>

#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
#include <vcclr.h>

#include "Torrent.h"

using PicoTorrent::Core::Session;
using PicoTorrent::Core::Torrent;
using PicoTorrent::Core::TorrentEventArgs;

struct SessionCallbackHandler
{
    SessionCallbackHandler(gcroot<Session^> session)
        : _session(session)
    {
    }

    void OnTorrentAdded(const std::shared_ptr<picotorrent::core::torrent> &torrent)
    {
        auto t = gcnew Torrent(torrent.get());
        _session->RaiseTorrentAdded(gcnew TorrentEventArgs(t));
    }

private:
    gcroot<Session^> _session;
};

Session::Session(picotorrent::core::session* sess)
    : sess_(sess),
    callbackHandler_(new SessionCallbackHandler(this))
{
    sess_->on_torrent_added().connect(std::bind(&SessionCallbackHandler::OnTorrentAdded, *callbackHandler_, std::placeholders::_1));
}

void Session::RaiseTorrentAdded(TorrentEventArgs^ args)
{
    TorrentAdded(this, args);
}
