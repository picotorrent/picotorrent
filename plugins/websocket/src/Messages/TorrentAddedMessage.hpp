#pragma once

#include <map>
#include <string>

#include <picotorrent/api.hpp>

namespace Messages
{
    struct TorrentAddedMessage
    {
        TorrentAddedMessage(Torrent const& torrent);
        std::string Serialize();

    private:
        Torrent m_torrent;
    };
}
