#pragma once

#include <map>
#include <string>

#include <picotorrent/api.hpp>

namespace Messages
{
    struct TorrentsUpdatedMessage
    {
        TorrentsUpdatedMessage(std::vector<Torrent> const& torrents);
        std::string Serialize();

    private:
        std::vector<Torrent> m_torrents;
    };
}
