#include "PicoStateMessage.hpp"

#include <picojson.hpp>

#include "../Serialization/TorrentSerializer.hpp"

namespace pj = picojson;
using Messages::PicoStateMessage;

PicoStateMessage::PicoStateMessage(std::map<std::string, Torrent> const& torrents)
    : m_torrents(torrents)
{
}

std::string PicoStateMessage::Serialize()
{
    pj::object obj;
    obj["type"] = pj::value("pico_state");

    pj::object vi;
    vi["api_version"] = pj::value(static_cast<int64_t>(1));

    pj::array torrents;

    for (auto &torrent : m_torrents)
    {
        pj::object t = Serialization::TorrentSerializer::Serialize(torrent.second);
        torrents.push_back(pj::value(t));
    }

    obj["torrents"] = pj::value(torrents);
    obj["version_info"] = pj::value(vi);

    return pj::value(obj).serialize();
}
