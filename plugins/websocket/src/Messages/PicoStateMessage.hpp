#pragma once

#include <map>
#include <string>

#include <picotorrent/api.hpp>

namespace Messages
{
    struct PicoStateMessage
    {
        PicoStateMessage(std::map<std::string, Torrent> const& torrents);
        std::string Serialize();

    private:
        std::map<std::string, Torrent> m_torrents;
    };
}
