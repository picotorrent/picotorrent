#include "PicoStateMessage.hpp"

#include <picojson.hpp>

#include "../Serialization/TorrentSerializer.hpp"

namespace pj = picojson;
using Messages::PicoStateMessage;

PicoStateMessage::PicoStateMessage(std::map<std::string, std::shared_ptr<Torrent>> const& torrents)
    : m_torrents(torrents)
{
}

std::string PicoStateMessage::Serialize()
{
    pj::object obj;
    obj["type"] = pj::value("pico_state");

    pj::array torrents;

    for (auto &torrent : m_torrents)
    {
        pj::object t = Serialization::TorrentSerializer::Serialize(torrent.second);
        torrents.push_back(pj::value(t));
    }

    obj["torrents"] = pj::value(torrents);

    return pj::value(obj).serialize();
}
