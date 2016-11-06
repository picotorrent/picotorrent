#include "TorrentSerializer.hpp"

#include <picotorrent/utils.hpp>

namespace pj = picojson;
using Serialization::TorrentSerializer;

pj::object TorrentSerializer::Serialize(std::shared_ptr<Torrent> torrent)
{
    pj::object obj;
    obj["info_hash"] = pj::value(torrent->infoHash);
    obj["name"] = pj::value(torrent->name);
    obj["queue_position"] = pj::value(static_cast<int64_t>(torrent->queuePosition));
    obj["size"] = pj::value(torrent->size);
    obj["progress"] = pj::value(torrent->progress);
   //  obj["status"] = pj::value(torrent->state().to_string());
   //  obj["eta"] = pj::value((int64_t)torrent->eta());
    obj["dl_rate"] = pj::value(static_cast<int64_t>(torrent->downloadRate));
    obj["ul_rate"] = pj::value(static_cast<int64_t>(torrent->uploadRate));
    obj["seeds_connected"] = pj::value(static_cast<int64_t>(torrent->seedsConnected));
    obj["seeds_total"] = pj::value(static_cast<int64_t>(torrent->seedsTotal));
    obj["nonseeds_connected"] = pj::value(static_cast<int64_t>(torrent->nonseedsConnected));
    obj["nonseeds_total"] = pj::value(static_cast<int64_t>(torrent->nonseedsTotal));

    return obj;
}
