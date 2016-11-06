#pragma once

#include <picojson.hpp>
#include <picotorrent/api.hpp>

namespace Serialization
{
    class TorrentSerializer
    {
    public:
        static picojson::object Serialize(std::shared_ptr<Torrent> torrent);
    };
}
