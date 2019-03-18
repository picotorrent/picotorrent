#pragma once

#include <QObject>

#include <memory>
#include <vector>

#include <libtorrent/download_priority.hpp>
#include <libtorrent/fwd.hpp>
#include <libtorrent/sha1_hash.hpp>

namespace pt
{
    class Session;
    struct TorrentStatus;

    class TorrentHandle : public QObject
    {
        Q_OBJECT

        friend class Session;

    public:
        virtual ~TorrentHandle();

        void fileProgress(std::vector<std::int64_t>& progress, int flags) const;
        std::vector<libtorrent::download_priority_t> getFilePriorities() const;
        void getPeerInfo(std::vector<libtorrent::peer_info>& peers) const;
        libtorrent::sha1_hash infoHash();
        TorrentStatus status();
        std::vector<libtorrent::announce_entry> trackers() const;

    public slots:
        void forceReannounce();
        void forceRecheck();
        void moveStorage(QString const& newPath);
        void pause();
        void queueUp();
        void queueDown();
        void queueTop();
        void queueBottom();
        void remove();
        void removeFiles();
        void resume();
        void resumeForce();
        void setFilePriorities(std::vector<libtorrent::download_priority_t> priorities);
        void setFilePriority(libtorrent::file_index_t index, libtorrent::download_priority_t priority);

    private:
        TorrentHandle(Session* session, libtorrent::torrent_handle const& th);
        void updateStatus(libtorrent::torrent_status const& ts);
        libtorrent::torrent_handle& wrappedHandle();

        Session* m_session;
        std::unique_ptr<libtorrent::torrent_handle> m_th;
        std::unique_ptr<libtorrent::torrent_status> m_ts;
    };
}
