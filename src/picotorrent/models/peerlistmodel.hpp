#pragma once

#include <QAbstractListModel>

#include <memory>
#include <vector>

#include <libtorrent/fwd.hpp>

class QPixmap;

namespace pt
{
    class GeoIP;
    class TorrentHandle;

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

        PeerListModel(GeoIP* geo);
        virtual ~PeerListModel();

        void clear();
        void update(TorrentHandle* torrent);

        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex&, int role) const override;
        QVariant headerData(int section, Qt::Orientation, int role) const override;
        QModelIndex parent(const QModelIndex&);
        int rowCount(const QModelIndex&) const override;

    private:
        std::vector<libtorrent::peer_info> m_peers;

        GeoIP* m_geo;
        QPixmap* m_flags;
    };
}
