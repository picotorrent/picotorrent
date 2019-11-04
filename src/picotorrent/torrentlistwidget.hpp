#pragma once

#include <memory>

#include <QList>
#include <QTreeView>

#include "torrenthandle.hpp"

class QAbstractItemModel;
class QAction;
class QItemSelection;
class QPoint;

namespace pt
{
    class Database;

    class TorrentListWidget : public QTreeView
    {
        Q_OBJECT

    public:
        TorrentListWidget(QWidget* parent, QAbstractItemModel* model, std::shared_ptr<Database> db);
        virtual ~TorrentListWidget();

        QSize sizeHint() const override;

    signals:
        void torrentsDeselected(QList<TorrentHandle*> torrents);
        void torrentsSelected(QList<TorrentHandle*> torrents);

    private slots:
        void showTorrentExplorer(QModelIndex const& index);
        void torrentSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);

    private:
        void showHeaderContextMenu(QPoint const& point);
        void toggleColumnVisibility(QAction* action);

        std::shared_ptr<Database> m_db;
    };
}
