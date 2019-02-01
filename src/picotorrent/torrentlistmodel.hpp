#pragma once

#include <unordered_set>
#include <vector>

#include <QAbstractListModel>

#include <libtorrent/fwd.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_status.hpp>

namespace pt
{
    class TorrentHandle;

    class TorrentListModel : public QAbstractListModel
    {
    public:
        enum Columns
        {
            Name,
            QueuePosition,
            Size,
            Status,
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

        TorrentListModel();
        virtual ~TorrentListModel();

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    public slots:
        void addTorrent(TorrentHandle* torrent);
        void removeTorrent(TorrentHandle* torrent);
        void updateTorrent(TorrentHandle* torrent);

    private:
        std::vector<TorrentHandle*> m_torrents;
    };
}
