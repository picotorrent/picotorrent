#pragma once

#include <vector>

#include <picotorrent/api.hpp>

namespace Commands
{
    struct RemoveTorrentsCommand
    {
        bool removeData;
        std::vector<Torrent> torrents;
    };
}
