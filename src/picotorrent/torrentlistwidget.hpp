#pragma once

#include <memory>

#include <QList>
#include <QTreeView>

class QAction;
class QItemSelection;
class QPoint;

namespace pt
{
    class Database;
    class Torrent;
    class TorrentListModel;

    class TorrentListWidget : public QTreeView
    {
        Q_OBJECT

    public:
        TorrentListWidget(QWidget* parent, TorrentListModel* model, std::shared_ptr<Database> db);
        ~TorrentListWidget();

        QSize sizeHint() const override;

    signals:
        void torrentsSelected(QList<Torrent*> torrents);

    private slots:
        void torrentSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);

    private:
        void showHeaderContextMenu(QPoint const& point);
        void showTorrentContextMenu(QPoint const& point);
        void toggleColumnVisibility(QAction* action);

        std::shared_ptr<Database> m_db;
        TorrentListModel* m_model;
    };
}
