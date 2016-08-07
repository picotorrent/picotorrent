#include "Session.h"

#include <functional>

#include <picotorrent/common/config/configuration.hpp>
#include <picotorrent/core/add_request.hpp>
#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/torrent_info.hpp>

#include <msclr/marshal_cppstd.h>
#include <vcclr.h>

#include "Torrent.h"

typedef std::shared_ptr<picotorrent::core::torrent> torrent_ptr;

using PicoTorrent::Core::Session;
using PicoTorrent::Core::Torrent;
using PicoTorrent::Core::TorrentEventArgs;

struct SessionCallbackHandler
{
    SessionCallbackHandler(gcroot<Session^> session)
        : _session(session)
    {
    }

    void OnTorrentAdded(const torrent_ptr &torrent)
    {
        auto t = gcnew Torrent(torrent.get());
        _session->RaiseTorrentAdded(gcnew TorrentEventArgs(t));
    }

    void OnTorrentFinished(const torrent_ptr &torrent)
    {
        auto t = gcnew Torrent(torrent.get());
        _session->RaiseTorrentFinished(gcnew TorrentEventArgs(t));
    }

    void OnTorrentRemoved(const torrent_ptr &torrent)
    {
        auto t = gcnew Torrent(torrent.get());
        _session->RaiseTorrentRemoved(gcnew TorrentEventArgs(t));
    }

private:
    gcroot<Session^> _session;
};

Session::Session(picotorrent::core::session* sess)
    : sess_(sess),
    callbackHandler_(new SessionCallbackHandler(this))
{
    sess_->on_torrent_added().connect(std::bind(&SessionCallbackHandler::OnTorrentAdded, *callbackHandler_, std::placeholders::_1));
    sess_->on_torrent_finished().connect(std::bind(&SessionCallbackHandler::OnTorrentFinished, *callbackHandler_, std::placeholders::_1));
    sess_->on_torrent_removed().connect(std::bind(&SessionCallbackHandler::OnTorrentRemoved, *callbackHandler_, std::placeholders::_1));
}

void Session::AddTorrent(System::String^ torrentFile, System::String^ savePath)
{
    std::string err;
    auto ti = picotorrent::core::torrent_info::try_load(
        msclr::interop::marshal_as<std::string>(torrentFile),
        err);

    if (!ti || !err.empty())
    {
        throw gcnew System::Exception(msclr::interop::marshal_as<System::String^>(err));
    }

    auto req = std::make_shared<picotorrent::core::add_request>();
    req->set_save_path(picotorrent::common::config::configuration::instance().default_save_path());
    req->set_torrent_info(*ti);

    if (!System::String::IsNullOrEmpty(savePath))
    {
        req->set_save_path(msclr::interop::marshal_as<std::string>(savePath));
    }

    sess_->add_torrent(req);
}

void Session::RaiseTorrentAdded(TorrentEventArgs^ args)
{
    TorrentAdded(this, args);
}

void Session::RaiseTorrentFinished(TorrentEventArgs^ args)
{
    TorrentFinished(this, args);
}

void Session::RaiseTorrentRemoved(TorrentEventArgs^ args)
{
    TorrentRemoved(this, args);
}
