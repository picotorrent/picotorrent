#pragma once

#include <memory>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

typedef websocketpp::server<websocketpp::config::asio_tls> websocketpp_server;
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

namespace picotorrent
{
namespace client
{
namespace ws
{
    class websocket_server
    {
    public:
        websocket_server();
        ~websocket_server();

    private:
        std::shared_ptr<websocketpp_server> srv_;
    };
}
}
}
