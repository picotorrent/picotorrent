#include <picotorrent/server/application.hpp>

#include <picotorrent/core/session.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/server/net/websocket_server.hpp>
#include <picotorrent/server/net/messages/torrent_updated_message.hpp>

namespace msg = picotorrent::server::net::messages;
using picotorrent::core::session;
using picotorrent::core::torrent;
using picotorrent::server::application;
using picotorrent::server::net::websocket_server;

application::application()
    : io_(),
    sess_(std::make_shared<session>()),
    signals_(io_, SIGINT, SIGTERM),
    timer_(io_),
    ws_(std::make_shared<websocket_server>(io_))
{
    sess_->on_notifications_available().connect(std::bind(&application::notifications_available, this));
    sess_->on_torrent_updated().connect(std::bind(&application::torrent_updated, this, std::placeholders::_1));

    signals_.async_wait([this](const boost::system::error_code &ec, int signal)
    {
        io_.stop();
    });
}

application::~application()
{
}

int application::run()
{
    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(std::bind(&application::timer_elapsed, this, std::placeholders::_1));

    ws_->start();
    sess_->load();

    io_.run();

    sess_->unload();

    return 0;
}

void application::notifications_available()
{
    io_.dispatch([this]
    {
        sess_->notify();
    });
}

void application::timer_elapsed(const boost::system::error_code &ec)
{
    sess_->post_updates();
    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(std::bind(&application::timer_elapsed, this, std::placeholders::_1));
}

void application::torrent_updated(const std::shared_ptr<torrent> &torrent)
{
    io_.dispatch([this, torrent]
    {
        msg::torrent_updated_message upd(torrent);
        ws_->broadcast(upd);
    });
}
