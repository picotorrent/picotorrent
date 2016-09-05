#pragma once

#include <vector>

namespace Models { struct Torrent; }

namespace Commands
{
    struct PauseTorrentsCommand
    {
        std::vector<Models::Torrent> torrents;
    };
}
