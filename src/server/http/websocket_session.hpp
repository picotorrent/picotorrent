#pragma once

#include <memory>
#include <queue>
#include <string>

#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace pt
{
    class session_manager;
}

namespace pt::http
{
    class websocket_session : public std::enable_shared_from_this<websocket_session>
    {
    public:
        explicit websocket_session(
            boost::asio::ip::tcp::socket&& socket,
            sqlite3* db,
            std::shared_ptr<session_manager> session);

        template<class Body, class Allocator>
        void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
        {
            // Set suggested timeout settings for the websocket
            m_websocket.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

            // Set a decorator to change the Server of the handshake
            m_websocket.set_option(boost::beast::websocket::stream_base::decorator(
                [](boost::beast::websocket::response_type& res)
                {
                    res.set(boost::beast::http::field::server, "bt-dl/1.0");
                }));

            // Accept the websocket handshake
            m_websocket.async_accept(
                req,
                boost::beast::bind_front_handler(
                    &websocket_session::begin_accept,
                    shared_from_this()));
        }

    private:
        void begin_accept(boost::system::error_code ec);
        void begin_read();
        void end_read(boost::system::error_code ec, size_t bytes_transferred);
        void end_write(boost::system::error_code ec, size_t bytes_transferred);
        void maybe_write();
        void on_subscribe(nlohmann::json& j);

        boost::beast::websocket::stream<boost::beast::tcp_stream> m_websocket;
        boost::beast::flat_buffer m_buffer;
        sqlite3* m_db;
        std::shared_ptr<session_manager> m_session;
        bool m_is_writing{ false };
        std::queue<std::string> m_send_data;
        std::shared_ptr<void> m_subscriber_tag;
    };
}
