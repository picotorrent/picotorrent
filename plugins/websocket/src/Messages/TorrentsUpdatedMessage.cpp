#include "TorrentsUpdatedMessage.hpp"

#include <picojson.hpp>

#include "../Serialization/TorrentSerializer.hpp"

namespace pj = picojson;
using Messages::TorrentsUpdatedMessage;

TorrentsUpdatedMessage::TorrentsUpdatedMessage(std::vector<Torrent> const& torrents)
    : m_torrents(torrents)
{
}

std::string TorrentsUpdatedMessage::Serialize()
{
    pj::object obj;
    obj["type"] = pj::value("torrent_updated");

    pj::array torrents;

    for (auto &torrent : m_torrents)
    {
        pj::object t = Serialization::TorrentSerializer::Serialize(torrent);
        torrents.push_back(pj::value(t));
    }

    obj["torrents"] = pj::value(torrents);

    return pj::value(obj).serialize();
}
