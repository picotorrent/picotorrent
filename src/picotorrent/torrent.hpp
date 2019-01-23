#pragma once

#include <chrono>
#include <memory>

#include <QDateTime>
#include <QObject>
#include <QString>

#include <libtorrent/fwd.hpp>

namespace pt
{
    class Session;

    class Torrent : public QObject
    {
        Q_OBJECT

        friend class Session;

    public:
        virtual ~Torrent();

        QDateTime addedOn();
        float availability();
        QString availabilityFormatted();
        QDateTime completedOn();
        int downloadRate();
        QString downloadRateFormatted();
        std::chrono::seconds eta();
        QString etaFormatted();
        QString infoHash();
        QString name();
        int peersCurrent();
        int peersTotal();
        float progress();
        int queuePosition();
        float ratio();
        QString ratioFormatted();
        int seedsCurrent();
        int seedsTotal();
        QString sizeTotalFormatted();
        int uploadRate();
        QString uploadRateFormatted();

    private:
        Torrent(QObject* parent, libtorrent::torrent_handle const& th);

        void updateStatus(libtorrent::torrent_status const& ts);

        std::unique_ptr<libtorrent::torrent_handle> m_th;
        std::unique_ptr<libtorrent::torrent_status> m_ts;
    };
}
