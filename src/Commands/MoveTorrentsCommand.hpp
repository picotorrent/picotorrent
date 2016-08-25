#pragma once

#include <vector>

namespace Models { class Torrent; }

namespace Commands
{
    struct MoveTorrentsCommand
    {
        std::vector<Models::Torrent> torrents;
    };
}
