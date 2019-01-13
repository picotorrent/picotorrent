#pragma once

#include <memory>

#include "detailstab.hpp"

class QTreeView;

namespace pt
{
    class GeoIP;
    class PeerListModel;
    struct SessionState;

    class TorrentPeersWidget : public DetailsTab
    {
    public:
        TorrentPeersWidget(std::shared_ptr<SessionState> state, std::shared_ptr<GeoIP> geo);

        virtual void clear() override;
        virtual void refresh() override;

    private:
        std::shared_ptr<SessionState> m_state;

        QTreeView* m_peersView;
        PeerListModel* m_peersModel;
    };
}
