#pragma once

#include <memory>

#include "detailstab.hpp"

class QTreeView;

namespace pt
{
    struct SessionState;
    class TrackersListModel;

    class TorrentTrackersWidget : public DetailsTab
    {
    public:
        TorrentTrackersWidget(std::shared_ptr<SessionState> state);

        virtual void clear() override;
        virtual void refresh() override;

    private:
        std::shared_ptr<SessionState> m_state;

        QTreeView* m_trackersView;
        TrackersListModel* m_trackersModel;
    };
}
