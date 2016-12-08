#pragma once

#include <map>
#include <string>

#include <picotorrent/api.hpp>

namespace Messages
{
    struct TorrentFinishedMessage
    {
        TorrentFinishedMessage(Torrent const& torrent);
        std::string Serialize();

    private:
        Torrent m_torrent;
    };
}
