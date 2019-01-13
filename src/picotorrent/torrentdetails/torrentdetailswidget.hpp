#pragma once

#include <QTabWidget>

#include <memory>
#include <vector>

class QWidget;

namespace pt
{
    class GeoIP;
    struct SessionState;
    class TorrentFilesWidget;
    class TorrentOverviewWidget;
    class TorrentPeersWidget;
    class TorrentTrackersWidget;

    class TorrentDetailsWidget : public QTabWidget
    {
    public:
        TorrentDetailsWidget(QWidget* parent, std::shared_ptr<SessionState> state, std::shared_ptr<GeoIP> geo);

        void clear();
        void refresh();

    private:
        std::shared_ptr<SessionState> m_state;

        TorrentOverviewWidget* m_overview;
        TorrentFilesWidget* m_files;
        TorrentPeersWidget* m_peers;
        TorrentTrackersWidget* m_trackers;
    };
}
