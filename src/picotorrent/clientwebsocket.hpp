#pragma once

#include <boost/beast.hpp>
#include <memory>

namespace pt
{
    class ClientWebSocket : public std::enable_shared_from_this<ClientWebSocket>
    {
    public:
        ClientWebSocket(boost::asio::io_context& io);
        ~ClientWebSocket();

        void Run();

    private:
        void BeginRead();
        void OnConnect(boost::beast::error_code ec);
        void OnHandshake(boost::beast::error_code ec);
        void OnRead(boost::beast::error_code ec, size_t bytes_transferred);

        boost::beast::websocket::stream<boost::beast::tcp_stream> m_websocket;
        boost::beast::flat_buffer m_buffer;
    };
}
