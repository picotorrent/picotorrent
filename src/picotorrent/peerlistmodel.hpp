#pragma once

#include <QAbstractListModel>

#include <vector>

#include <libtorrent/fwd.hpp>

namespace pt
{
    class PeerListModel : public QAbstractListModel
    {
    public:
        enum Columns
        {
            IP,
            Client,
            Flags,
            DownloadRate,
            UploadRate,
            Progress,
            _Max
        };

        PeerListModel();
        virtual ~PeerListModel();

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    private:
        std::vector<libtorrent::peer_info> m_peers;
    };
}
