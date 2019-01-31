#include "torrentdetailswidget.hpp"

#include "../sessionstate.hpp"
#include "../torrenthandle.hpp"
#include "../translator.hpp"

#include "detailstab.hpp"
#include "torrentfileswidget.hpp"
#include "torrentoverviewwidget.hpp"
#include "torrentpeerswidget.hpp"
#include "torrenttrackerswidget.hpp"

using pt::TorrentDetailsWidget;

TorrentDetailsWidget::TorrentDetailsWidget(QWidget* parent, std::shared_ptr<pt::SessionState> state, pt::GeoIP* geo)
    : QTabWidget(parent),
    m_state(state)
{
    m_overview = new TorrentOverviewWidget();
    m_files    = new TorrentFilesWidget();
    m_peers    = new TorrentPeersWidget(geo);
    m_trackers = new TorrentTrackersWidget();

    QTabWidget::addTab(m_overview, i18n("overview"));
    QTabWidget::addTab(m_files,    i18n("files"));
    QTabWidget::addTab(m_peers,    i18n("peers"));
    QTabWidget::addTab(m_trackers, i18n("trackers"));

    this->setMinimumHeight(140);
    this->setMovable(false);
}

void TorrentDetailsWidget::update(QList<pt::TorrentHandle*> const& torrents)
{
    for (int i = 0; i < this->count(); i++)
    {
        QWidget* currWidget = this->widget(i);
        DetailsTab* tab = dynamic_cast<DetailsTab*>(currWidget);

        if (torrents.count() > 0)
        {
            tab->refresh(torrents);
        }
        else
        {
            tab->clear();
        }
    }
}
