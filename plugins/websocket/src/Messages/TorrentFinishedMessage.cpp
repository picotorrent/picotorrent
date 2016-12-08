#include "TorrentFinishedMessage.hpp"

#include <picojson.hpp>

#include "../Serialization/TorrentSerializer.hpp"

namespace pj = picojson;
using Messages::TorrentFinishedMessage;

TorrentFinishedMessage::TorrentFinishedMessage(Torrent const& torrent)
    : m_torrent(torrent)
{
}

std::string TorrentFinishedMessage::Serialize()
{
    pj::object obj;
    obj["type"] = pj::value("torrent_finished");

    pj::object t = Serialization::TorrentSerializer::Serialize(m_torrent);
    obj["torrent"] = pj::value(t);

    return pj::value(obj).serialize();
}
