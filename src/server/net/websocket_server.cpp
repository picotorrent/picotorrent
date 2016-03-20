#include <picotorrent/server/net/websocket_server.hpp>

#include <picotorrent/server/net/messages/message.hpp>

using picotorrent::server::net::websocket_server;
using picotorrent::server::net::messages::message;

websocket_server::websocket_server(boost::asio::io_service &io)
{
    srv_.init_asio(&io);
    srv_.set_close_handler(std::bind(&websocket_server::on_close, this, std::placeholders::_1));
    srv_.set_open_handler(std::bind(&websocket_server::on_open, this, std::placeholders::_1));
}

websocket_server::~websocket_server()
{
}

void websocket_server::broadcast(const message &message)
{
    std::ostringstream ss;
    message.write_to(ss);

    for (auto it : conns_)
    {
        srv_.send(it, ss.str(), websocketpp::frame::opcode::TEXT);
    }
}

void websocket_server::start()
{
    srv_.listen(7071);
    srv_.start_accept();
}

void websocket_server::on_close(websocketpp::connection_hdl connection)
{
    conns_.erase(connection);
    printf("Connection closed");
}

void websocket_server::on_open(websocketpp::connection_hdl connection)
{
    conns_.insert(connection);
    printf("Connection opened");
}
