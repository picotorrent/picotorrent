#pragma once

#include <unordered_set>
#include <vector>

#include <QAbstractListModel>

#include <libtorrent/fwd.hpp>
#include <libtorrent/sha1_hash.hpp>

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

        void addTorrent(libtorrent::torrent_status const& status);
        void removeTorrent(libtorrent::sha1_hash const& infoHash);
        void updateTorrent(libtorrent::torrent_status const& status);
        void appendInfoHashes(QModelIndexList const& indexes, std::unordered_set<libtorrent::sha1_hash>& hashes);

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    private:
        std::chrono::seconds getEta(libtorrent::torrent_status const& status) const;
        float getRatio(libtorrent::torrent_status const& status) const;

        std::vector<libtorrent::torrent_status> m_status;
    };
}
