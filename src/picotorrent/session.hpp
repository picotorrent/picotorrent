#pragma once

#include <QObject>

#include <map>
#include <memory>

#include <libtorrent/fwd.hpp>
#include <libtorrent/sha1_hash.hpp>

class QTimer;

namespace pt
{
    class Configuration;
    class Database;
    class Torrent;

    class Session : public QObject
    {
        Q_OBJECT

    public:
        Session(QObject* parent, std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg);
        virtual ~Session();

        void addTorrent(libtorrent::add_torrent_params const& params);

    public slots:
        void reloadSettings();

    signals:
        void torrentAdded(Torrent* torrent);
        void torrentRemoved(Torrent* torrent);
        void torrentUpdated(Torrent* torrent);

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
        std::map<libtorrent::sha1_hash, Torrent*> m_torrents;

        QTimer* m_updateTimer;
    };
}
