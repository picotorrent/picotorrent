#pragma once

#include <memory>

#include <QList>
#include <QTreeView>

#include "torrenthandle.hpp"

class QAction;
class QItemSelection;
class QPoint;

namespace pt
{
    class Database;
    class TorrentListModel;

    class TorrentListWidget : public QTreeView
    {
        Q_OBJECT

    public:
        TorrentListWidget(QWidget* parent, TorrentListModel* model, std::shared_ptr<Database> db);
        virtual ~TorrentListWidget();

        QSize sizeHint() const override;

    signals:
        void torrentsSelected(QList<TorrentHandle*> torrents);

    private slots:
        void torrentSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);

    private:
        void showHeaderContextMenu(QPoint const& point);
        void toggleColumnVisibility(QAction* action);

        std::shared_ptr<Database> m_db;
        TorrentListModel* m_model;
    };
}
