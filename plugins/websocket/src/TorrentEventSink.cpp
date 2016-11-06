#include "TorrentEventSink.hpp"

#include "Messages/PicoStateMessage.hpp"
#include "Messages/TorrentsUpdatedMessage.hpp"

TorrentEventSink::TorrentEventSink()
    : m_wss(std::make_shared<websocketpp_server>())
{
    m_wss->init_asio(&m_io);
	m_wss->set_close_handler(std::bind(&TorrentEventSink::OnCloseConnection, this, std::placeholders::_1));
	m_wss->set_open_handler(std::bind(&TorrentEventSink::OnOpenConnection, this, std::placeholders::_1));

    // Start our thread
    m_worker = std::thread(std::bind(&TorrentEventSink::Run, this));
}

void TorrentEventSink::OnTorrentAdded(Torrent torrent)
{
	m_io.post([this, torrent]()
	{
		m_torrents.insert({ torrent.infoHash, torrent });
	});
}

void TorrentEventSink::OnTorrentRemoved(std::string const& infoHash)
{
	m_io.post([this, infoHash]()
	{
		m_torrents.erase(infoHash);
	});
}

void TorrentEventSink::OnTorrentUpdated(std::vector<Torrent> torrents)
{
	m_io.post([this, torrents]()
	{
		for (Torrent t : torrents)
		{
			m_torrents.at(t.infoHash) = t;
		}

		Messages::TorrentsUpdatedMessage msg(torrents);
		Broadcast(msg.Serialize());
	});
}

void TorrentEventSink::Broadcast(std::string const& data)
{
	for (auto hdl : m_connections)
	{
		m_wss->get_con_from_hdl(hdl)
			->send(data);
	}
}

void TorrentEventSink::OnCloseConnection(websocketpp::connection_hdl connection)
{
	m_connections.erase(connection);
}

void TorrentEventSink::OnOpenConnection(websocketpp::connection_hdl connection)
{
	m_connections.insert(connection);

    Messages::PicoStateMessage msg(m_torrents);
    m_wss->get_con_from_hdl(connection)
         ->send(msg.Serialize());
}

void TorrentEventSink::Run()
{
    m_wss->listen(7878);
    m_wss->start_accept();
    m_wss->run();
}
