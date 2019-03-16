#pragma once

#include <QObject>

#include <map>
#include <memory>

#include <libtorrent/fwd.hpp>
#include <libtorrent/session_types.hpp>
#include <libtorrent/sha1_hash.hpp>

class QTimer;

namespace pt
{
    class Configuration;
    class Database;
    struct SessionStatistics;
    class TorrentHandle;
    struct TorrentStatistics;

    class Session : public QObject
    {
        Q_OBJECT

    public:
        Session(QObject* parent, std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg);
        virtual ~Session();

        void addTorrent(libtorrent::add_torrent_params const& params);
        void removeTorrent(TorrentHandle* handle, libtorrent::remove_flags_t flags = {});

    public slots:
        void reloadSettings();

    signals:
        void sessionStatsUpdated(SessionStatistics* stats);
        void torrentAdded(TorrentHandle* torrent);
        void torrentRemoved(TorrentHandle* torrent);
        void torrentStatsUpdated(TorrentStatistics* stats);
        void torrentUpdated(TorrentHandle* torrent);

    private slots:
        void readAlerts();

    private:
        void loadState();
        void loadTorrents();
        void postUpdates();
        void saveState();
        void saveTorrents();

        std::unique_ptr<libtorrent::session> m_session;
        std::shared_ptr<Database> m_db;
        std::shared_ptr<Configuration> m_cfg;

        std::map<libtorrent::sha1_hash, TorrentHandle*> m_torrents;

        QTimer* m_updateTimer;
    };
}
