#include <picotorrent/client/ws/websocket_server.hpp>

using picotorrent::client::ws::websocket_server;

websocket_server::websocket_server()
    : srv_(std::make_shared<websocketpp_server>())
{
}

websocket_server::~websocket_server()
{
}
