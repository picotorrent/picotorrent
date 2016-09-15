#pragma once

#include "../Models/Torrent.hpp"

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
        Models::Torrent torrent;
    };
}
