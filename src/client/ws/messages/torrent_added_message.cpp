#include <picotorrent/client/ws/messages/torrent_added_message.hpp>

#include <picojson.hpp>
#include <picotorrent/client/ws/serialization/torrent_serializer.hpp>
#include <picotorrent/core/torrent.hpp>

namespace pj = picojson;
using picotorrent::client::ws::messages::torrent_added_message;
using picotorrent::client::ws::serialization::torrent_serializer;
using picotorrent::core::torrent;

torrent_added_message::torrent_added_message(const std::shared_ptr<torrent> &torrent)
    : torrent_(torrent)
{
}

std::string torrent_added_message::serialize()
{
    pj::object obj;
    obj["type"] = pj::value("torrent_added");

    pj::object t = torrent_serializer::serialize(torrent_);
    obj["torrent"] = pj::value(t);

    return pj::value(obj).serialize();
}
