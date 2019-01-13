#include "torrentpeerswidget.hpp"

#include <QTreeView>
#include <QVBoxLayout>

#include <libtorrent/torrent_handle.hpp>

#include "../peerlistmodel.hpp"
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

TorrentPeersWidget::TorrentPeersWidget(std::shared_ptr<pt::SessionState> state, std::shared_ptr<pt::GeoIP> geo)
    : m_state(state)
{
    m_peersModel = new PeerListModel(geo);
    m_peersView = new MinimumTreeView();
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
}

void TorrentPeersWidget::refresh()
{
    if (m_state->selectedTorrents.size() != 1)
    {
        return;
    }

    auto hash = (*m_state->selectedTorrents.begin());
    auto th = m_state->torrents.at(hash);

    m_peersModel->update(th);
}
