#pragma once

#include <memory>

#include "detailstab.hpp"

class QTreeView;

namespace pt
{
    struct SessionState;
    class Torrent;
    class TrackersListModel;

    class TorrentTrackersWidget : public DetailsTab
    {
    public:
        TorrentTrackersWidget();

        virtual void clear() override;
        virtual void refresh(QList<Torrent*> const& torrents) override;

    private:
        QTreeView* m_trackersView;
        TrackersListModel* m_trackersModel;
    };
}
