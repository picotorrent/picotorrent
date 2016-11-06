#include "TorrentEventSink.hpp"

#include "Messages/PicoStateMessage.hpp"

TorrentEventSink::TorrentEventSink()
    : m_wss(std::make_shared<websocketpp_server>())
{
    m_wss->init_asio(&m_io);
    m_wss->set_open_handler(std::bind(&TorrentEventSink::OnOpenConnection, this, std::placeholders::_1));

    // Start our thread
    m_worker = std::thread(std::bind(&TorrentEventSink::Run, this));
}

void TorrentEventSink::OnTorrentAdded(std::shared_ptr<Torrent> torrent)
{
    m_torrents.insert({ torrent->infoHash, torrent });
}

void TorrentEventSink::OnTorrentRemoved(std::string const& infoHash)
{
    m_torrents.erase(infoHash);
}

void TorrentEventSink::OnTorrentUpdated(std::shared_ptr<Torrent> torrent)
{
	m_torrents.at(torrent->infoHash) = torrent;
}

void TorrentEventSink::OnOpenConnection(websocketpp::connection_hdl connection)
{
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
