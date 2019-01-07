#pragma once

#include <memory>

#include <QTreeView>

#include <picotorrent.hpp>

class QAction;
class QPoint;

namespace pt
{
    class Database;
    class TorrentListModel;

    class TorrentListWidget : public QTreeView, public ITorrentListWidget
    {
    public:
        TorrentListWidget(QWidget* parent, TorrentListModel* model, std::shared_ptr<Database> db);
        ~TorrentListWidget();

        QSize sizeHint() const override;

    private:
        void showHeaderContextMenu(QPoint const& point);
        void showTorrentContextMenu(QPoint const& point);
        void toggleColumnVisibility(QAction* action);

        std::shared_ptr<Database> m_db;
        TorrentListModel* m_model;
    };
}
