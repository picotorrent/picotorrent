#include "clientwebsocket.hpp"

#include <boost/asio.hpp>

using pt::ClientWebSocket;

ClientWebSocket::ClientWebSocket(boost::asio::io_context& io)
    : m_websocket(boost::asio::make_strand(io))
{
}

ClientWebSocket::~ClientWebSocket()
{
    OutputDebugStringA("WS: Destroyed\n");
}

void ClientWebSocket::Run()
{
    boost::beast::get_lowest_layer(m_websocket).expires_after(std::chrono::seconds(30));
    boost::beast::get_lowest_layer(m_websocket).async_connect(
        boost::asio::ip::tcp::endpoint{ boost::asio::ip::make_address("127.0.0.1"), 6545 },
        boost::beast::bind_front_handler(
            &ClientWebSocket::OnConnect,
            shared_from_this()));
}

void ClientWebSocket::BeginRead()
{
    m_websocket.async_read(
        m_buffer,
        boost::beast::bind_front_handler(
            &ClientWebSocket::OnRead,
            shared_from_this()));
}

void ClientWebSocket::OnConnect(boost::beast::error_code ec)
{
    if (ec)
    {
        OutputDebugStringA(ec.message().c_str());
        OutputDebugStringA("\n");
        return;
    }

    boost::beast::get_lowest_layer(m_websocket).expires_never();
    m_websocket.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::client));

    OutputDebugStringA("WS: Connected\n");

    m_websocket.async_handshake("127.0.0.1:6545", "/api/ws",
        boost::beast::bind_front_handler(
            &ClientWebSocket::OnHandshake,
            shared_from_this()));
}

void ClientWebSocket::OnHandshake(boost::beast::error_code ec)
{
    if (ec)
    {
        OutputDebugStringA(ec.message().c_str());
        OutputDebugStringA("\n");
        return;
    }

    OutputDebugStringA("WS: Handshake done\n");

    BeginRead();
}

void ClientWebSocket::OnRead(boost::beast::error_code ec, size_t bytes_transferred)
{
    if (ec)
    {
        OutputDebugStringA(ec.message().c_str());
        OutputDebugStringA("\n");
        return;
    }

    OutputDebugStringA("WS: Read ");
    OutputDebugStringA(std::to_string(bytes_transferred).c_str());
    OutputDebugStringA(" bytes\n");

    OutputDebugStringA("WS: ");
    OutputDebugStringA(boost::beast::buffers_to_string(m_buffer.data()).c_str());
    OutputDebugStringA("\n");

    BeginRead();
}
