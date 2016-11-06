#pragma once

#include <map>
#include <memory>
#include <thread>

#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning(pop)

#include <picotorrent/api.hpp>

typedef websocketpp::server<websocketpp::config::asio> websocketpp_server;
typedef websocketpp_server::message_ptr message_ptr;

class TorrentEventSink : public ITorrentEventSink
{
public:
    TorrentEventSink();

    void OnTorrentAdded(std::shared_ptr<Torrent> torrent);
    void OnTorrentRemoved(std::string const& infoHash);

private:
    void OnOpenConnection(websocketpp::connection_hdl connection);
    void Run();

    std::map<std::string, std::shared_ptr<Torrent>> m_torrents;

    boost::asio::io_service m_io;
    std::shared_ptr<websocketpp_server> m_wss;
    std::thread m_worker;
};
