#pragma once

#include <map>
#include <string>

#include <picotorrent/api.hpp>

namespace Messages
{
    struct TorrentRemovedMessage
    {
        TorrentRemovedMessage(std::string const& infoHash);
        std::string Serialize();

    private:
        std::string m_infoHash;
    };
}
