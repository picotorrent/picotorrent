#pragma once

#include "../Models/Torrent.hpp"

namespace Commands
{
    struct ShowTorrentDetailsCommand
    {
        Models::Torrent torrent;
    };
}
