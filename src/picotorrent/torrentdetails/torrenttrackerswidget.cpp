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

static QString trackerUrlSchemes[] = {"http", "https", "udp"};

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

    QClipboard* clipboard = QGuiApplication::clipboard();
    QString text = clipboard->text();
    if (isTrackerUrl(text))
    {
        dlg->setText(text);
    }

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
    auto modelData = m_trackersView->model()->data(index, Qt::UserRole);
    auto item = modelData.value<TrackersListModel::ListItem>();

    if (m_torrents.size() > 0)
    {
        auto trackers = m_torrents.at(0)->trackers();
        auto found = std::find_if(
            trackers.begin(),
            trackers.end(),
            [&item](lt::announce_entry const& e)
            {
                return e.url == item.key
                    && e.tier == item.tier
                    && !item.isTier;
            });

        if (found != trackers.end())
        {
            trackers.erase(found);

            m_torrents.at(0)->replaceTrackers(trackers);
            m_trackersModel->update(m_torrents.at(0));
        }
    }
}

void TorrentTrackersWidget::onRemoveTier()
{
    auto index = m_trackersView->selectionModel()->currentIndex();
    auto modelData = m_trackersView->model()->data(index, Qt::UserRole);
    auto item = modelData.value<TrackersListModel::ListItem>();

    if (m_torrents.size() > 0)
    {
        auto trackers = m_torrents.at(0)->trackers();

        trackers.erase(
            std::remove_if(
                trackers.begin(),
                trackers.end(),
                [item](lt::announce_entry const& ae)
                {
                    return ae.tier == item.tier;
                }),
            trackers.end());

        m_torrents.at(0)->replaceTrackers(trackers);
        m_trackersModel->update(m_torrents.at(0));
    }
}

void TorrentTrackersWidget::onForceReannounce()
{
    auto index = m_trackersView->selectionModel()->currentIndex();
    auto modelData = m_trackersView->model()->data(index, Qt::UserRole);
    auto item = modelData.value<TrackersListModel::ListItem>();

    if (m_torrents.size() > 0)
    {
        auto trackers = m_torrents.at(0)->trackers();
        auto found = std::find_if(
            trackers.begin(),
            trackers.end(),
            [&item](lt::announce_entry const& e)
            {
                return e.url == item.key
                    && e.tier == item.tier
                    && !item.isTier;
            });

        if (found != trackers.end())
        {
            m_torrents[0]->forceReannounce(
                0,
                std::distance(trackers.begin(), found));

            m_trackersModel->update(m_torrents.at(0));
        }
    }
}

void TorrentTrackersWidget::onScrape()
{
    auto index = m_trackersView->selectionModel()->currentIndex();
    auto modelData = m_trackersView->model()->data(index, Qt::UserRole);
    auto item = modelData.value<TrackersListModel::ListItem>();

    if (m_torrents.size() > 0)
    {
        auto trackers = m_torrents.at(0)->trackers();
        auto found = std::find_if(
            trackers.begin(),
            trackers.end(),
            [&item](lt::announce_entry const& e)
            {
                return e.url == item.key
                    && e.tier == item.tier
                    && !item.isTier;
            });

        if (found != trackers.end())
        {
            m_torrents[0]->scrapeTracker(
                std::distance(trackers.begin(), found));

            m_trackersModel->update(m_torrents.at(0));
        }
    }
}

void TorrentTrackersWidget::onTrackerContextMenu(QPoint const& point)
{
    if (m_torrents.size() <= 0)
    {
        return;
    }

    QMenu*      menu  = new QMenu();
    QModelIndex index = m_trackersView->indexAt(point);

    QObject::connect(menu, &QMenu::aboutToHide,
                     menu, &QMenu::deleteLater);

    if (index.isValid())
    {
        auto modelData = m_trackersView->model()->data(index, Qt::UserRole);
        auto item = modelData.value<TrackersListModel::ListItem>();

        if (item.tier < 0)
        {
            QObject::disconnect(menu, &QMenu::aboutToHide,
                                menu, &QMenu::deleteLater);

            delete menu;

            return;
        }

        if (item.isTier)
        {
            auto removeTier = new QAction(i18n("remove_tier"), menu);

            QObject::connect(removeTier, &QAction::triggered,
                             this,       &TorrentTrackersWidget::onRemoveTier);

            menu->addAction(removeTier);
        }
        else
        {
            auto copyUrl         = new QAction(i18n("copy_url"), menu);
            auto forceReannounce = new QAction(i18n("force_reannounce"), menu);
            auto scrape          = new QAction(i18n("scrape"), menu);
            auto remove          = new QAction(i18n("remove"), menu);

            QObject::connect(copyUrl, &QAction::triggered,
                             this,    &TorrentTrackersWidget::onCopyUrl);

            QObject::connect(forceReannounce, &QAction::triggered,
                             this,            &TorrentTrackersWidget::onForceReannounce);

            QObject::connect(scrape, &QAction::triggered,
                             this,   &TorrentTrackersWidget::onScrape);

            QObject::connect(remove, &QAction::triggered,
                             this,   &TorrentTrackersWidget::onRemove);

            menu->addAction(copyUrl);
            menu->addAction(forceReannounce);
            menu->addAction(scrape);
            menu->addSeparator();
            menu->addAction(remove);
        }
    }
    else
    {
        auto addTracker = new QAction(i18n("add_tracker"));

        QObject::connect(addTracker, &QAction::triggered,
                         this,       &TorrentTrackersWidget::onAddTracker);

        menu->addAction(addTracker);
    }

    menu->popup(m_trackersView->viewport()->mapToGlobal(point));
}

bool TorrentTrackersWidget::isTrackerUrl(QString url)
{
    for (const QString& urlScheme : trackerUrlSchemes)
    {
        if (url.startsWith(urlScheme + "://", Qt::CaseSensitivity::CaseInsensitive))
        {
            return true;
        }
    }

    return false;
}
