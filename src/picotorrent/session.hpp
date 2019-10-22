#pragma once

#include <QObject>

#include <map>
#include <memory>
#include <vector>

#include <libtorrent/fwd.hpp>
#include <libtorrent/info_hash.hpp>
#include <libtorrent/session_types.hpp>

class QTimer;

namespace pt
{
    class Configuration;
    class Database;
    class Environment;
    struct SessionStatistics;
    class TorrentHandle;
    struct TorrentStatistics;

    class Session : public QObject
    {
        Q_OBJECT

    public:
        Session(QObject* parent, std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg, std::shared_ptr<Environment> env);
        virtual ~Session();

        void addTorrent(libtorrent::add_torrent_params const& params);
        void metadataSearch(std::vector<libtorrent::info_hash_t> const& hashes);
        void removeTorrent(TorrentHandle* handle, libtorrent::remove_flags_t flags = {});

    public slots:
        void reloadSettings();

    signals:
        void metadataSearchResult(std::shared_ptr<libtorrent::torrent_info>* ti);
        void sessionStatsUpdated(SessionStatistics* stats);
        void torrentAdded(TorrentHandle* torrent);
        void torrentFinished(TorrentHandle* torrent);
        void torrentRemoved(TorrentHandle* torrent);
        void torrentStatsUpdated(TorrentStatistics* stats);
        void torrentUpdated(TorrentHandle* torrent);

    private slots:
        void readAlerts();

    private:
        void loadState();
        void loadTorrents();
        void loadTorrentsOld();
        void postUpdates();
        void saveState();
        void saveTorrents();

        std::unique_ptr<libtorrent::session> m_session;
        std::shared_ptr<Database> m_db;
        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<Environment> m_env;

        std::map<libtorrent::info_hash_t, TorrentHandle*> m_torrents;
        std::map<libtorrent::info_hash_t, libtorrent::torrent_handle> m_metadataSearches;

        QTimer* m_updateTimer;
    };
}
