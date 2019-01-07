#include "torrentdetailswidget.hpp"

#include "../sessionstate.hpp"
#include "../translator.hpp"

#include "detailstab.hpp"
#include "torrentfileswidget.hpp"
#include "torrentoverviewwidget.hpp"
#include "torrentpeerswidget.hpp"
#include "torrenttrackerswidget.hpp"

using pt::TorrentDetailsWidget;

TorrentDetailsWidget::TorrentDetailsWidget(QWidget* parent, std::shared_ptr<pt::SessionState> state)
    : QTabWidget(parent),
    m_state(state)
{
    m_overview = new TorrentOverviewWidget(state);
    m_files = new TorrentFilesWidget(state);
    m_peers = new TorrentPeersWidget(state);
    m_trackers = new TorrentTrackersWidget(state);

    QTabWidget::addTab(m_overview, i18n("overview"));
    QTabWidget::addTab(m_files,    i18n("files"));
    QTabWidget::addTab(m_peers,    i18n("peers"));
    QTabWidget::addTab(m_trackers, i18n("trackers"));

    this->setMinimumHeight(140);
    this->setMovable(false);

    // Refresh when we change tab
    connect(this, &QTabWidget::currentChanged, [=] { this->refresh(); });
}

void TorrentDetailsWidget::addTab(pt::Tab const& tab)
{
}

void TorrentDetailsWidget::clear()
{
    QWidget* currWidget = this->currentWidget();
    DetailsTab* tab = dynamic_cast<DetailsTab*>(currWidget);
    tab->clear();
}

void TorrentDetailsWidget::refresh()
{
    QWidget* currWidget = this->currentWidget();
    DetailsTab* tab = dynamic_cast<DetailsTab*>(currWidget);
    tab->refresh();
}
