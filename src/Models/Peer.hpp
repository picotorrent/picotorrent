#pragma once

#include <memory>
#include <string>

namespace libtorrent
{
    struct peer_info;
}

namespace Models
{
    struct Peer
    {
        static Peer Map(const std::wstring& endpoint);
        static Peer Map(const libtorrent::peer_info& peer);

        bool operator==(const Peer& other)
        {
            return endpoint == other.endpoint;
        }

        bool operator!=(const Peer& other)
        {
            return !(*this == other);
        }

        std::wstring endpoint;
        std::wstring client;
        std::wstring flags;
        int downloadRate;
        int uploadRate;
        float progress;
    };
}
