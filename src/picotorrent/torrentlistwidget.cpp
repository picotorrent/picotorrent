#include "torrentlistwidget.hpp"

#include <picotorrent/core/database.hpp>

#include <QHeaderView>
#include <QMenu>

#include "torrentitemdelegate.hpp"
#include "torrentlistmodel.hpp"

using pt::TorrentListWidget;

TorrentListWidget::TorrentListWidget(QWidget* parent, pt::TorrentListModel* model, std::shared_ptr<pt::Database> db)
    : m_db(db),
    m_model(model)
{
    this->setModel(model);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setItemDelegate(new TorrentItemDelegate(this));
    this->setRootIsDecorated(false);
    this->setSortingEnabled(true);

    auto header = this->header();
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    header->setFirstSectionMovable(true);
    header->setSectionsMovable(true);
    header->setStretchLastSection(false);

    // These are default sizes for the columns. Real values are set from the database.
    header->resizeSection(TorrentListModel::Columns::Name,          160);
    header->resizeSection(TorrentListModel::Columns::QueuePosition, 30);
    header->resizeSection(TorrentListModel::Columns::Size,          60);

    auto torrentListQ = m_db->statement("SELECT column_id, width, position, is_visible FROM column_state WHERE list_id = ?");
    torrentListQ->bind(1, "torrent_list");

    while (torrentListQ->read())
    {
        int columnId = torrentListQ->getInt(0);
        int width = torrentListQ->getInt(1);
        int position = torrentListQ->getInt(2);
        bool visible = torrentListQ->getBool(3);

        header->moveSection(header->visualIndex(columnId), position);
        header->resizeSection(columnId, width);

        if (!visible)
        {
            header->hideSection(columnId);
        }
    }

    auto sm = this->selectionModel();

    QObject::connect(header, &QHeaderView::customContextMenuRequested,
                     this,   &TorrentListWidget::showHeaderContextMenu);

    QObject::connect(this,   &QTreeView::customContextMenuRequested,
                     this,   &TorrentListWidget::showTorrentContextMenu);

    QObject::connect(sm,     &QItemSelectionModel::selectionChanged,
                     this,   &TorrentListWidget::torrentSelectionChanged);
}

TorrentListWidget::~TorrentListWidget()
{
    const char* sql = "INSERT INTO column_state (list_id, column_id, position, width, is_visible) VALUES (?, ?, ?, ?, ?)\n"
        "ON CONFLICT(list_id, column_id) DO UPDATE SET position = excluded.position, width = excluded.width, is_visible = excluded.is_visible;";

    auto header = this->header();

    for (int i = 0; i < header->count(); i++)
    {
        bool sectionHidden = header->isSectionHidden(i);
        int sectionSize = header->sectionSize(i);

        if (sectionHidden)
        {
            header->showSection(i);
            sectionSize = header->sectionSize(i);
        }

        auto stmt = m_db->statement(sql);
        stmt->bind(1, "torrent_list");
        stmt->bind(2, i);
        stmt->bind(3, header->visualIndex(i));
        stmt->bind(4, sectionSize);
        stmt->bind(5, !sectionHidden);
        stmt->execute();
    }
}

QSize TorrentListWidget::sizeHint() const
{
    return QSize(-1, 120);
}

void TorrentListWidget::showHeaderContextMenu(QPoint const& point)
{
    auto header = this->header();
    auto model = this->model();

    QMenu menu;

    for (int i = 0; i < header->count(); i++)
    {
        int logicalIndex = header->logicalIndex(i);

        QVariant data = model->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole);

        QAction* action = menu.addAction(data.toString());
        action->setCheckable(true);
        action->setChecked(!header->isSectionHidden(logicalIndex));
        action->setData(logicalIndex);
    }

    connect(
        &menu,
        &QMenu::triggered,
        this,
        &TorrentListWidget::toggleColumnVisibility);

    menu.exec(header->viewport()->mapToGlobal(point));
}

void TorrentListWidget::showTorrentContextMenu(QPoint const& point)
{
    auto sel = this->selectedIndexes();

    if (sel.size() > 0)
    {
        printf("hej");
    }
}

void TorrentListWidget::torrentSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected)
{
    QList<Torrent*> torrents;

    for (QModelIndex const& idx : selected.indexes())
    {
        if (idx.column() > 0)
        {
            continue;
        }

        auto variant = this->model()->data(idx, Qt::UserRole);
        auto torrent = static_cast<Torrent*>(variant.value<void*>());

        torrents.append(torrent);
    }

    emit torrentsSelected(torrents);
}

void TorrentListWidget::toggleColumnVisibility(QAction* action)
{
    auto header = this->header();
    int index = action->data().toInt();

    if (header->isSectionHidden(index))
    {
        header->showSection(index);
    }
    else
    {
        header->hideSection(index);
    }
}
