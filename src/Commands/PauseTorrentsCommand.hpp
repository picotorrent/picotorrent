#pragma once

#include <vector>

#include <picotorrent/api.hpp>

namespace Commands
{
    struct PauseTorrentsCommand
    {
        std::vector<Torrent> torrents;
    };
}
