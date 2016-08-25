#pragma once

#include <vector>

namespace Models { class Torrent; }

namespace Commands
{
    struct PauseTorrentsCommand
    {
        std::vector<Models::Torrent> torrents;
    };
}
