#include "torrenttrackerswidget.hpp"

#include <libtorrent/torrent_handle.hpp>

#include <QTreeView>
#include <QVBoxLayout>

#include "../models/trackerslistmodel.hpp"
#include "../sessionstate.hpp"

class MinimumTreeView : public QTreeView
{
public:
    QSize sizeHint() const override
    {
        return QSize(-1, 120);
    }
};

using pt::TorrentTrackersWidget;

TorrentTrackersWidget::TorrentTrackersWidget()
{
    m_trackersModel = new TrackersListModel();
    m_trackersView = new MinimumTreeView();
    m_trackersView->setModel(m_trackersModel);
    m_trackersView->setRootIsDecorated(false);

    auto layout = new QVBoxLayout();
    layout->addWidget(m_trackersView);
    layout->setContentsMargins(2, 2, 2, 2);

    this->clear();
    this->setLayout(layout);
}

void TorrentTrackersWidget::clear()
{
    m_trackersModel->clear();
}

void TorrentTrackersWidget::refresh(QList<pt::Torrent*> const& torrents)
{
    if (torrents.count() != 1)
    {
        return;
    }

    Torrent* torrent = torrents.at(0);

    // m_trackersModel->update(th);
}
