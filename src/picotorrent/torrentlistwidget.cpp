#include "torrentlistwidget.hpp"

#include <QHeaderView>
#include <QMenu>

#include "database.hpp"
#include "torrentitemdelegate.hpp"
#include "torrentlistmodel.hpp"

using pt::TorrentListWidget;

TorrentListWidget::TorrentListWidget(QWidget* parent, pt::TorrentListModel* model, std::shared_ptr<pt::Database> db)
    : QTreeView(parent),
    m_db(db),
    m_model(model)
{
    this->setModel(model);
    this->setAllColumnsShowFocus(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setItemDelegate(new TorrentItemDelegate());
    this->setRootIsDecorated(false);
    this->setSortingEnabled(true);

    auto header = this->header();
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    header->setFirstSectionMovable(true);
    header->setSectionsMovable(true);

    // These are default sizes for the columns. Real values are set from the database.
    header->resizeSection(TorrentListModel::Columns::Name, 160);
    header->resizeSection(TorrentListModel::Columns::QueuePosition, 30);
    header->resizeSection(TorrentListModel::Columns::Size, 60);

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
    }

    connect(
        header,
        &QHeaderView::customContextMenuRequested,
        this,
        &TorrentListWidget::showHeaderContextMenu);

    connect(
        this,
        &QTreeView::customContextMenuRequested,
        this,
        &TorrentListWidget::showTorrentContextMenu);
}

TorrentListWidget::~TorrentListWidget()
{
    const char* sql = "INSERT INTO column_state (list_id, column_id, position, width) VALUES (?, ?, ?, ?)\n"
        "ON CONFLICT(list_id, column_id) DO UPDATE SET position = excluded.position, width = excluded.width;";

    auto header = this->header();

    for (int i = 0; i < header->count(); i++)
    {
        auto stmt = m_db->statement(sql);
        stmt->bind(1, "torrent_list");
        stmt->bind(2, i);
        stmt->bind(3, header->visualIndex(i));
        stmt->bind(4, header->sectionSize(i));
        stmt->execute();
    }
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
