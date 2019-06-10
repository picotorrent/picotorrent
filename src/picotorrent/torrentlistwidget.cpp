#include "torrentlistwidget.hpp"

#include <filesystem>

#include <QHeaderView>
#include <QMenu>
#include <QProcess>

#include "core/database.hpp"
#include "torrenthandle.hpp"
#include "torrentitemdelegate.hpp"
#include "torrentlistmodel.hpp"
#include "torrenthandle.hpp"
#include "torrentstatus.hpp"

namespace fs = std::experimental::filesystem;
using pt::TorrentListWidget;

TorrentListWidget::TorrentListWidget(QWidget* parent, QAbstractItemModel* model, std::shared_ptr<pt::Database> db)
    : m_db(db)
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

    // Hidden columns
    header->hideSection(TorrentListModel::Columns::SizeRemaining);

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

        if (visible)
        {
            header->showSection(columnId);
        }
        else
        {
            header->hideSection(columnId);
        }
    }

    // Sorting
    auto torrentSortListQ = m_db->statement("SELECT sort_column_index, sort_column_order FROM list_state WHERE id = ?");
    torrentSortListQ->bind(1, "torrent_list");

    if (torrentSortListQ->read())
    {
        int sortColumnIndex = torrentSortListQ->getInt(0);
        int sortColumnOrder = torrentSortListQ->getInt(1);

        this->sortByColumn(
            sortColumnIndex,
            static_cast<Qt::SortOrder>(sortColumnOrder));
    }

    QObject::connect(header,                 &QHeaderView::customContextMenuRequested,
                     this,                   &TorrentListWidget::showHeaderContextMenu);

    QObject::connect(this->selectionModel(), &QItemSelectionModel::selectionChanged,
                     this,                   &TorrentListWidget::torrentSelectionChanged);

    QObject::connect(this,                   &TorrentListWidget::activated,
                     this,                   &TorrentListWidget::showTorrentExplorer);
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

    auto sortStmt = m_db->statement("INSERT INTO list_state (id, sort_column_index, sort_column_order) VALUES (?, ?, ?) ON CONFLICT (id) DO UPDATE SET sort_column_index = excluded.sort_column_index, sort_column_order = excluded.sort_column_order");
    sortStmt->bind(1, "torrent_list");
    sortStmt->bind(2, header->sortIndicatorSection());
    sortStmt->bind(3, (int)header->sortIndicatorOrder());
    sortStmt->execute();
}

QSize TorrentListWidget::sizeHint() const
{
    return QSize(-1, 100);
}

void TorrentListWidget::showHeaderContextMenu(QPoint const& point)
{
    auto header = this->header();
    auto model = this->model();

    auto menu = new QMenu();

    for (int i = 0; i < header->count(); i++)
    {
        int logicalIndex = header->logicalIndex(i);

        QVariant data = model->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole);

        QAction* action = menu->addAction(data.toString());
        action->setCheckable(true);
        action->setChecked(!header->isSectionHidden(logicalIndex));
        action->setData(logicalIndex);
    }

    menu->popup(header->viewport()->mapToGlobal(point));

    QObject::connect(menu, &QMenu::triggered,
                     this, &TorrentListWidget::toggleColumnVisibility);

    QObject::connect(menu, &QMenu::aboutToHide,
                     menu, &QMenu::deleteLater);
}

void TorrentListWidget::showTorrentExplorer(QModelIndex const& index)
{
    auto variant = this->model()->data(index, Qt::UserRole);
    auto torrent = static_cast<TorrentHandle*>(variant.value<void*>());
    auto ts      = torrent->status();

    fs::path savePath = ts.savePath.toStdWString();
    fs::path path     = savePath / ts.name.toStdWString();

    QStringList param;

    if (!fs::is_directory(path))
    {
        param += QLatin1String("/select,");
    }

    param += QString::fromStdWString(fs::absolute(path).wstring());

    QProcess::startDetached("explorer.exe", param);
}

void TorrentListWidget::torrentSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected)
{
    QList<TorrentHandle*> torrents;

    for (QModelIndex const& idx : selected.indexes())
    {
        if (idx.column() > 0)
        {
            continue;
        }

        auto variant = this->model()->data(idx, Qt::UserRole);
        auto torrent = static_cast<TorrentHandle*>(variant.value<void*>());

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
