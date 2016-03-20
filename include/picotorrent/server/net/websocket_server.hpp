#pragma once

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <boost/asio.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

#include <set>

namespace picotorrent
{
namespace server
{
namespace net
{
namespace messages
{
    class message;
}

    class websocket_server
    {
    public:
        websocket_server(boost::asio::io_service &io);
        ~websocket_server();

        void broadcast(const messages::message &message);
        void start();

    private:
        void on_close(websocketpp::connection_hdl connection);
        void on_open(websocketpp::connection_hdl connection);

        websocketpp::server<websocketpp::config::asio> srv_;
        std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> conns_;
    };
}
}
}
