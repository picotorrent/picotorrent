#pragma once

#include <vector>

namespace Models { class Torrent; }

namespace Commands
{
    struct ResumeTorrentsCommand
    {
        bool force;
        std::vector<Models::Torrent> torrents;
    };
}
