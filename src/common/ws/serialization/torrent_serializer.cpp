#include <picotorrent/common/ws/serialization/torrent_serializer.hpp>

#include <picotorrent/core/hash.hpp>
#include <picotorrent/core/torrent.hpp>
#include <picotorrent/core/torrent_state.hpp>

namespace pj = picojson;
using picotorrent::common::ws::serialization::torrent_serializer;
using picotorrent::core::torrent;

pj::object torrent_serializer::serialize(const std::shared_ptr<torrent> &torrent)
{
    pj::object obj;
    obj["info_hash"] = pj::value(torrent->info_hash()->to_string());
    obj["name"] = pj::value(torrent->name());
    obj["queue_position"] = pj::value((int64_t)torrent->queue_position());
    obj["size"] = pj::value(torrent->size());
    obj["progress"] = pj::value(torrent->progress());
    obj["status"] = pj::value(torrent->state().to_string());
    obj["eta"] = pj::value((int64_t)torrent->eta());
    obj["dl_rate"] = pj::value((int64_t)torrent->download_rate());
    obj["ul_rate"] = pj::value((int64_t)torrent->upload_rate());
    obj["seeds_connected"] = pj::value((int64_t)torrent->connected_seeds());
    obj["seeds_total"] = pj::value((int64_t)torrent->total_seeds());
    obj["nonseeds_connected"] = pj::value((int64_t)torrent->connected_nonseeds());
    obj["nonseeds_total"] = pj::value((int64_t)torrent->total_nonseeds());

    return obj;
}
