#pragma once

#include <QTabWidget>

#include <picotorrent.hpp>

#include <memory>
#include <vector>

class QWidget;

namespace pt
{
    struct SessionState;
    class TorrentFilesWidget;
    class TorrentOverviewWidget;
    class TorrentPeersWidget;
    class TorrentTrackersWidget;

    class TorrentDetailsWidget : public QTabWidget, public ITorrentDetailsWidget
    {
    public:
        TorrentDetailsWidget(QWidget* parent, std::shared_ptr<SessionState> state);

        void addTab(Tab const& tab) override;
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
