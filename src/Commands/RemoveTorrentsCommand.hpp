#pragma once

#include <vector>

namespace Models { struct Torrent; }

namespace Commands
{
    struct RemoveTorrentsCommand
    {
        bool removeData;
        std::vector<Models::Torrent> torrents;
    };
}
