#include "torrentdetailswidget.hpp"
#include "ui_torrentdetailswidget.h"

#include "../sessionstate.hpp"
#include "../torrenthandle.hpp"
#include "../translator.hpp"

#include "detailstab.hpp"
#include "torrentfileswidget.hpp"
#include "torrentoverviewwidget.hpp"
#include "torrentpeerswidget.hpp"
#include "torrenttrackerswidget.hpp"

using pt::TorrentDetailsWidget;

TorrentDetailsWidget::TorrentDetailsWidget(QWidget* parent)
    : QTabWidget(parent),
    m_ui(new Ui::TorrentDetailsWidget())
{
    m_ui->setupUi(this);

    this->setTabText(0, i18n("overview"));
    this->setTabText(1, i18n("files"));
    this->setTabText(2, i18n("peers"));
    this->setTabText(3, i18n("trackers"));
}

void TorrentDetailsWidget::setGeo(pt::GeoIP* geo)
{
    m_ui->peers->setGeo(geo);
}

void TorrentDetailsWidget::update(QList<pt::TorrentHandle*> const& torrents)
{
    for (int i = 0; i < this->count(); i++)
    {
        QWidget* currWidget = this->widget(i);
        DetailsTab* tab = dynamic_cast<DetailsTab*>(currWidget);

        // For now, the details panel only supports showing information for a single torrent.
        if (torrents.count() == 1 && torrents.at(0)->isValid())
        {
            tab->refresh(torrents);
        }
        else
        {
            tab->clear();
        }
    }
}
