#pragma once

#include <map>
#include <string>

#include <picotorrent/api.hpp>

namespace Messages
{
    struct PicoStateMessage
    {
        PicoStateMessage(std::map<std::string, std::shared_ptr<Torrent>> const& torrents);
        std::string Serialize();

    private:
        std::map<std::string, std::shared_ptr<Torrent>> m_torrents;
    };
}
