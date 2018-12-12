#pragma once

#include <vector>

#include <QAbstractListModel>

#include <libtorrent/fwd.hpp>

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
            _Max
        };

        void addTorrent(libtorrent::torrent_status const& status);
        void updateTorrent(libtorrent::torrent_status const& status);

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    private:
        std::vector<libtorrent::torrent_status> m_status;
    };
}
