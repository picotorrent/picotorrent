#pragma once

#include <memory>
#include <string>
#include <thread>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

typedef websocketpp::server<websocketpp::config::asio_tls> websocketpp_server;
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef std::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

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

        void start();
        void stop();

    private:
        std::string get_certificate_file();
        std::string get_certificate_password();

        void on_message(websocketpp::connection_hdl hdl);
        context_ptr on_tls_init(websocketpp::connection_hdl hdl);

        void run();

        std::shared_ptr<websocketpp_server> srv_;
        std::thread bg_;
        
    };
}
}
}
