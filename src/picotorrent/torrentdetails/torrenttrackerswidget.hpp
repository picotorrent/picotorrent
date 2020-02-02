#pragma once

#include <memory>

#include "detailstab.hpp"

class QPoint;
class QTreeView;

namespace pt
{
    struct SessionState;
    class TorrentHandle;
    class TrackersListModel;

    class TorrentTrackersWidget : public DetailsTab
    {
    public:
        TorrentTrackersWidget();

        virtual void clear() override;
        virtual void refresh(QList<TorrentHandle*> const& torrents) override;

    private slots:
        void onAddTracker();
        void onCopyUrl();
        void onForceReannounce();
        void onRemove();
        void onTrackerContextMenu(QPoint const& point);

    private:
        QList<TorrentHandle*> m_torrents;
        QTreeView* m_trackersView;
        TrackersListModel* m_trackersModel;

        bool isTrackerUrl(QString url);
    };
}
