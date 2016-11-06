#pragma once

#include <vector>

#include <picotorrent/api.hpp>

namespace Commands
{
    struct ResumeTorrentsCommand
    {
        bool force;
        std::vector<Torrent> torrents;
    };
}
