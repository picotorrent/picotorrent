#pragma once

#include <vector>

#include <picotorrent/api.hpp>

namespace Commands
{
    struct MoveTorrentsCommand
    {
        std::vector<Torrent> torrents;
    };
}
