#include "torrenttrackerswidget.hpp"

#include <libtorrent/announce_entry.hpp>
#include <libtorrent/torrent_handle.hpp>

#include <QAction>
#include <QClipboard>
#include <QGuiApplication>
#include <QMenu>
#include <QTreeView>
#include <QVBoxLayout>

#include "../models/trackerslistmodel.hpp"
#include "../textinputdialog.hpp"
#include "../torrenthandle.hpp"
#include "../translator.hpp"

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
    m_trackersView  = new MinimumTreeView();
    m_trackersView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_trackersView->setModel(m_trackersModel);
    m_trackersView->setRootIsDecorated(false);

    auto layout = new QVBoxLayout();
    layout->addWidget(m_trackersView);
    layout->setContentsMargins(2, 2, 2, 2);

    QObject::connect(m_trackersView, &QTreeView::customContextMenuRequested,
                     this,           &TorrentTrackersWidget::onTrackerContextMenu);

    this->clear();
    this->setLayout(layout);
}

void TorrentTrackersWidget::clear()
{
    m_trackersModel->clear();
}

void TorrentTrackersWidget::refresh(QList<pt::TorrentHandle*> const& torrents)
{
    if (torrents.count() != 1)
    {
        return;
    }

    m_torrents = torrents;
    m_trackersModel->update(torrents.at(0));
}

void TorrentTrackersWidget::onAddTracker()
{
    auto dlg = new TextInputDialog(this, i18n("tracker_urls"));
    dlg->setWindowTitle(i18n("add_tracker"));
    dlg->open();

    QObject::connect(dlg,  &QDialog::accepted,
                     [this, dlg]()
                     {
                         TorrentHandle* torrent = m_torrents.at(0);
                         torrent->addTracker(lt::announce_entry(dlg->text().toStdString()));
                         m_trackersModel->update(torrent);
                     });
}

void TorrentTrackersWidget::onCopyUrl()
{
    auto index = m_trackersView->selectionModel()->currentIndex();

    if (m_torrents.size() > 0)
    {
        auto trackers = m_torrents.at(0)->trackers();

        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(QString::fromStdString(trackers.at(index.row()).url));
    }
}

void TorrentTrackersWidget::onRemove()
{
    auto index = m_trackersView->selectionModel()->currentIndex();

    if (m_torrents.size() > 0)
    {
        auto trackers = m_torrents.at(0)->trackers();
        trackers.erase(trackers.begin() + index.row());

        m_torrents.at(0)->replaceTrackers(trackers);
    }
}

void TorrentTrackersWidget::onForceReannounce()
{
    auto index = m_trackersView->selectionModel()->currentIndex();

    if (m_torrents.size() > 0)
    {
        m_torrents.at(0)->forceReannounce(0, index.row());
    }
}

void TorrentTrackersWidget::onTrackerContextMenu(QPoint const& point)
{
    if (m_torrents.size() <= 0)
    {
        return;
    }

    QModelIndex idx = m_trackersView->indexAt(point);

    auto menu = new QMenu();

    if (idx.isValid())
    {
        auto copyUrl         = new QAction(i18n("copy_url"));
        auto forceReannounce = new QAction(i18n("force_reannounce"));
        auto remove          = new QAction(i18n("remove"));

        QObject::connect(copyUrl,         &QAction::triggered,
                         this,            &TorrentTrackersWidget::onCopyUrl);

        QObject::connect(forceReannounce, &QAction::triggered,
                         this,            &TorrentTrackersWidget::onForceReannounce);

        QObject::connect(remove,          &QAction::triggered,
                         this,            &TorrentTrackersWidget::onRemove);

        menu->addAction(copyUrl);
        menu->addAction(forceReannounce);
        menu->addSeparator();
        menu->addAction(remove);
    }
    else
    {
        auto addTracker = new QAction(i18n("add_tracker"));

        QObject::connect(addTracker, &QAction::triggered,
                         this,       &TorrentTrackersWidget::onAddTracker);

        menu->addAction(addTracker);
    }

    menu->popup(m_trackersView->viewport()->mapToGlobal(point));

    QObject::connect(menu, &QMenu::aboutToHide,
                     menu, &QMenu::deleteLater);
}
