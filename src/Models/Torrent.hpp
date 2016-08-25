#pragma once

#include <memory>
#include <string>

namespace libtorrent { class sha1_hash; struct torrent_status; }

namespace Models
{
    class Torrent
    {
    public:
        Torrent(const libtorrent::sha1_hash& hash);
        Torrent(const libtorrent::sha1_hash& hash, const libtorrent::torrent_status& ts);
        Torrent(const Torrent& other);

        ~Torrent();

        Torrent& operator=(Torrent other);
        bool operator==(const Torrent& other);
        bool operator!=(const Torrent& other);

        friend void swap(Torrent& first, Torrent& second)
        {
            std::swap(first.m_hash, second.m_hash);
            std::swap(first.m_status, second.m_status);
        }

        libtorrent::sha1_hash infoHash() const;
        std::wstring name() const;
        int queuePosition() const;
        int64_t size() const;
        int status() const;
        float progress() const;
        int eta() const;
        int downloadRate() const;
        int uploadRate() const;
        int seedsConnected() const;
        int seedsTotal() const;
        int peersConnected() const;
        int peersTotal() const;
        float shareRatio() const;
        bool isPaused() const;
        std::wstring savePath() const;

    private:
        std::unique_ptr<libtorrent::sha1_hash> m_hash;
        std::unique_ptr<libtorrent::torrent_status> m_status;
    };
}
