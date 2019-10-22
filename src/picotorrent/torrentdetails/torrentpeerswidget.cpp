#include "torrentpeerswidget.hpp"

#include <QTreeView>
#include <QVBoxLayout>

#include <libtorrent/torrent_handle.hpp>

#include "../models/peerlistmodel.hpp"
#include "../sessionstate.hpp"
#include "../translator.hpp"

class MinimumTreeView : public QTreeView
{
public:
    QSize sizeHint() const override
    {
        return QSize(-1, 120);
    }
};

using pt::TorrentPeersWidget;

TorrentPeersWidget::TorrentPeersWidget(pt::GeoIP* geo)
{
    m_peersModel = new PeerListModel(geo);
    m_peersView  = new MinimumTreeView();

    m_peersView->setModel(m_peersModel);
    m_peersView->setRootIsDecorated(false);

    auto layout = new QVBoxLayout();
    layout->addWidget(m_peersView);
    layout->setContentsMargins(2, 2, 2, 2);

    this->clear();
    this->setLayout(layout);
}

void TorrentPeersWidget::clear()
{
    m_peersModel->clear();
}

void TorrentPeersWidget::refresh(QList<pt::TorrentHandle*> const& torrents)
{
    if (torrents.count() != 1)
    {
        return;
    }

    m_peersModel->update(torrents.at(0));
}
