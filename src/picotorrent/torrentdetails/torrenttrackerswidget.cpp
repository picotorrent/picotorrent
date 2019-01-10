#include "torrenttrackerswidget.hpp"

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

TorrentTrackersWidget::TorrentTrackersWidget(std::shared_ptr<pt::SessionState> state)
    : m_state(state)
{
    m_trackersModel = new TrackersListModel();
    m_trackersView = new MinimumTreeView();
    m_trackersView->setModel(m_trackersModel);

    auto layout = new QVBoxLayout();
    layout->addWidget(m_trackersView);
    layout->setContentsMargins(2, 2, 2, 2);

    this->clear();
    this->setLayout(layout);
}

void TorrentTrackersWidget::clear()
{
}

void TorrentTrackersWidget::refresh()
{
}
