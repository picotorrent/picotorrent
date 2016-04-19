#include <picotorrent/client/ws/messages/torrent_updated_message.hpp>

#include <picojson.hpp>
#include <picotorrent/client/ws/serialization/torrent_serializer.hpp>
#include <picotorrent/core/torrent.hpp>

namespace pj = picojson;
using picotorrent::client::ws::messages::torrent_updated_message;
using picotorrent::client::ws::serialization::torrent_serializer;
using picotorrent::core::torrent;

torrent_updated_message::torrent_updated_message(const std::vector<std::shared_ptr<torrent>> &torrents)
    : torrents_(torrents)
{
}

std::string torrent_updated_message::serialize()
{
    pj::object obj;
    obj["type"] = pj::value("torrent_updated");

    pj::array torrents;

    for (auto &t : torrents_)
    {
        pj::object torrent = torrent_serializer::serialize(t);
        torrents.push_back(pj::value(torrent));
    }

    obj["torrents"] = pj::value(torrents);

    return pj::value(obj).serialize();
}
