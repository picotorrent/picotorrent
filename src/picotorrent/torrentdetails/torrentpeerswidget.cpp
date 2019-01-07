#include "torrentpeerswidget.hpp"

#include <QTreeView>
#include <QVBoxLayout>

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

TorrentPeersWidget::TorrentPeersWidget(std::shared_ptr<pt::SessionState> state)
    : m_state(state)
{
    m_peersModel = new PeerListModel();
    m_peersView = new MinimumTreeView();
    m_peersView->setModel(m_peersModel);

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
}
