#pragma once

#include <unordered_set>
#include <vector>

#include <QAbstractListModel>

#include <libtorrent/fwd.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_status.hpp>

#include "torrent.hpp"

namespace pt
{
    class TorrentListModel : public QAbstractListModel
    {
    public:
        enum Columns
        {
            Name,
            QueuePosition,
            Size,
            Progress,
            ETA,
            DownloadSpeed,
            UploadSpeed,
            Availability,
            Ratio,
            Seeds,
            Peers,
            AddedOn,
            CompletedOn,
            _Max
        };

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    public slots:
        void addTorrent(Torrent* torrent);
        void removeTorrent(Torrent* torrent);
        void updateTorrent(Torrent* torrent);

    private:
        std::vector<libtorrent::torrent_status> m_status;
        std::vector<Torrent*> m_torrents;
    };
}
