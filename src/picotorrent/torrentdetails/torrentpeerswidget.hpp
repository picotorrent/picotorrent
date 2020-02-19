#pragma once

#include <memory>

#include "detailstab.hpp"

class QTreeView;

namespace pt
{
    class GeoIP;
    class PeerListModel;
    class TorrentHandle;

    class TorrentPeersWidget : public DetailsTab
    {
    public:
        TorrentPeersWidget();

        virtual void clear() override;
        virtual void refresh(QList<TorrentHandle*> const& torrents) override;
        void setGeo(GeoIP* geo);

    private:
        QTreeView* m_peersView;
        PeerListModel* m_peersModel;
    };
}
