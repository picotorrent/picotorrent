#include "TorrentRemovedMessage.hpp"

#include <picojson.hpp>

#include "../Serialization/TorrentSerializer.hpp"

namespace pj = picojson;
using Messages::TorrentRemovedMessage;

TorrentRemovedMessage::TorrentRemovedMessage(std::string const& infoHash)
    : m_infoHash(infoHash)
{
}

std::string TorrentRemovedMessage::Serialize()
{
    pj::object obj;
    obj["type"] = pj::value("torrent_removed");
    obj["info_hash"] = pj::value(m_infoHash);

    return pj::value(obj).serialize();
}
