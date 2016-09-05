#pragma once

#include <vector>

namespace Models { struct Torrent; }

namespace Commands
{
    struct MoveTorrentsCommand
    {
        std::vector<Models::Torrent> torrents;
    };
}
