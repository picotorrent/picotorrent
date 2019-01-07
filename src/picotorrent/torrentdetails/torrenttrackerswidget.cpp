#include "torrenttrackerswidget.hpp"

#include "../sessionstate.hpp"

using pt::TorrentTrackersWidget;

TorrentTrackersWidget::TorrentTrackersWidget(std::shared_ptr<pt::SessionState> state)
    : m_state(state)
{
}

void TorrentTrackersWidget::clear()
{
}

void TorrentTrackersWidget::refresh()
{
}
