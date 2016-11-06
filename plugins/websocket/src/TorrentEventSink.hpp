#pragma once

#include <map>
#include <memory>
#include <set>
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

    void OnTorrentAdded(Torrent torrent);
    void OnTorrentRemoved(std::string const& infoHash);
	void OnTorrentUpdated(std::vector<Torrent> torrents);

private:
	void Broadcast(std::string const& data);
	void OnCloseConnection(websocketpp::connection_hdl connection);
	void OnOpenConnection(websocketpp::connection_hdl connection);
    void Run();

    std::map<std::string, Torrent> m_torrents;

    boost::asio::io_service m_io;
	std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> m_connections;
    std::shared_ptr<websocketpp_server> m_wss;
    std::thread m_worker;
};
