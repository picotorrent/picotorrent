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
        Peer(const std::wstring& endpoint);
        Peer(const std::wstring& endpoint, const libtorrent::peer_info& info);
        Peer(const Peer& other);
        ~Peer();

        Peer& operator=(Peer other);
        bool operator==(const Peer& other);
        bool operator!=(const Peer& other);

        friend void swap(Peer& first, Peer& second)
        {
            std::swap(first.m_endpoint, second.m_endpoint);
            std::swap(first.m_info, second.m_info);
        }

        std::wstring GetEndpoint();
        std::wstring GetClient();
        std::wstring GetFlags();
        int GetDownloadRate();
        int GetUploadRate();

    private:
        std::wstring m_endpoint;
        std::unique_ptr<libtorrent::peer_info> m_info;
    };
}
