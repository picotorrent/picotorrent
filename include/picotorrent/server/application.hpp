#pragma once

#include <boost/asio.hpp>
#include <memory>

namespace picotorrent
{
namespace core
{
    class session;
    class torrent;
}
namespace server
{
namespace net
{
    class websocket_server;
}

    class application
    {
    public:
        application();
        ~application();

        int run();

    private:
        void notifications_available();
        void timer_elapsed(const boost::system::error_code &ec);
        void torrent_updated(const std::shared_ptr<core::torrent>&);

        boost::asio::io_service io_;
        boost::asio::signal_set signals_;
        boost::asio::deadline_timer timer_;
        std::shared_ptr<core::session> sess_;
        std::shared_ptr<net::websocket_server> ws_;
    };
}
}
