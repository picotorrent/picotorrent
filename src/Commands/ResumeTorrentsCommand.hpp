#pragma once

#include <vector>

namespace Models { struct Torrent; }

namespace Commands
{
    struct ResumeTorrentsCommand
    {
        bool force;
        std::vector<Models::Torrent> torrents;
    };
}
