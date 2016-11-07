#include "TorrentAddedMessage.hpp"

#include <picojson.hpp>

#include "../Serialization/TorrentSerializer.hpp"

namespace pj = picojson;
using Messages::TorrentAddedMessage;

TorrentAddedMessage::TorrentAddedMessage(Torrent const& torrent)
    : m_torrent(torrent)
{
}

std::string TorrentAddedMessage::Serialize()
{
    pj::object obj;
    obj["type"] = pj::value("torrent_added");

    pj::object t = Serialization::TorrentSerializer::Serialize(m_torrent);
    obj["torrent"] = pj::value(t);

    return pj::value(obj).serialize();
}
