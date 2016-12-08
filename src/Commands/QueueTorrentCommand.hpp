#pragma once

#include <picotorrent/api.hpp>

namespace Commands
{
    struct QueueTorrentCommand
    {
        enum Direction
        {
            Up,
            Down,
            Top,
            Bottom
        };

        Direction direction;
        Torrent torrent;
    };
}
