#pragma once

#include <memory>
#include <string>

namespace libtorrent
{
    struct peer_info;
    class sha1_hash;
}

namespace Models
{
    struct Peer
    {
        Peer(const libtorrent::peer_info& info);
        Peer(const Peer& other);
        ~Peer();

        Peer& operator=(Peer other);
        bool operator==(const Peer& other);
        bool operator!=(const Peer& other);

        friend void swap(Peer& first, Peer& second)
        {
            std::swap(first.m_info, second.m_info);
        }

        std::wstring endpoint();

    private:
        std::unique_ptr<libtorrent::peer_info> m_info;
    };
}
