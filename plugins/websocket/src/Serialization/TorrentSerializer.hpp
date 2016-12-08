#pragma once

#include <picojson.hpp>
#include <picotorrent/api.hpp>

namespace Serialization
{
    class TorrentSerializer
    {
    public:
        static picojson::object Serialize(Torrent const& torrent);
    };
}
