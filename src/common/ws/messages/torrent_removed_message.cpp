#include <picotorrent/common/ws/messages/torrent_removed_message.hpp>

#include <picojson.hpp>

namespace pj = picojson;
using picotorrent::common::ws::messages::torrent_removed_message;

torrent_removed_message::torrent_removed_message(const std::string &hash)
    : hash_(hash)
{
}

std::string torrent_removed_message::serialize()
{
    pj::object obj;
    obj["type"] = pj::value("torrent_removed");
    obj["info_hash"] = pj::value(hash_);

    return pj::value(obj).serialize();
}
