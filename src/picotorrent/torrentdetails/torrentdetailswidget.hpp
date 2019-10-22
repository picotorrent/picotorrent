#pragma once

#include <QList>
#include <QTabWidget>

#include <memory>
#include <vector>

class QWidget;

namespace pt
{
    class GeoIP;
    struct SessionState;
    class TorrentHandle;
    class TorrentFilesWidget;
    class TorrentOverviewWidget;
    class TorrentPeersWidget;
    class TorrentTrackersWidget;

    class TorrentDetailsWidget : public QTabWidget
    {
    public:
        TorrentDetailsWidget(QWidget* parent, std::shared_ptr<SessionState> state, GeoIP* geo);

    public slots:
        void update(QList<TorrentHandle*> const& torrents);

    private:
        std::shared_ptr<SessionState> m_state;

        TorrentOverviewWidget* m_overview;
        TorrentFilesWidget* m_files;
        TorrentPeersWidget* m_peers;
        TorrentTrackersWidget* m_trackers;
    };
}
