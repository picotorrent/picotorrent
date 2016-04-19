#pragma once

#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include <picotorrent/_aux/disable_3rd_party_warnings.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <picotorrent/_aux/enable_3rd_party_warnings.hpp>

typedef websocketpp::server<websocketpp::config::asio_tls> websocketpp_server;
typedef websocketpp::config::asio::message_type::ptr message_ptr;
typedef std::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

namespace picotorrent
{
namespace core
{
    class session;
    class torrent;
}
namespace client
{
namespace ws
{
    class websocket_server
    {
    public:
        websocket_server(const std::shared_ptr<core::session> &session);
        ~websocket_server();

        bool is_listening();
        void start();
        void stop();

    private:
        bool is_running_;
        std::string get_certificate_file();
        std::string get_certificate_password();

        void on_close(websocketpp::connection_hdl hdl);
        void on_message(websocketpp::connection_hdl hdl);
        void on_open(websocketpp::connection_hdl hdl);
        bool on_validate(websocketpp::connection_hdl hdl);
        context_ptr on_tls_init(websocketpp::connection_hdl hdl);

        void on_torrent_added(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_removed(const std::shared_ptr<core::torrent> &torrent);
        void on_torrent_updated(const std::vector<std::shared_ptr<core::torrent>> &torrents);

        void run();

        boost::asio::io_service io_;
        std::string configured_token_;
        std::shared_ptr<websocketpp_server> srv_;
        std::thread bg_;
        std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> connections_;

        // We should probably not store the torrents and session directly
        // on top of the WebSocket server, but lets do it for now.
        std::shared_ptr<core::session> session_;
        std::vector<std::shared_ptr<core::torrent>> torrents_;
    };
}
}
}
