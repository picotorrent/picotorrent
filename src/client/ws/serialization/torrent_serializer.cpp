#include <picotorrent/client/ws/serialization/torrent_serializer.hpp>

#include <picotorrent/core/torrent.hpp>

namespace pj = picojson;
using picotorrent::client::ws::serialization::torrent_serializer;
using picotorrent::core::torrent;

pj::object torrent_serializer::serialize(const std::shared_ptr<torrent> &torrent)
{
    pj::object obj;
    obj["name"] = pj::value(torrent->name());

    return obj;
}
