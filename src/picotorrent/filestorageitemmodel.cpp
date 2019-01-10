#include "filestorageitemmodel.hpp"

#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_info.hpp>

#include <QFileIconProvider>
#include <QFileInfo>

#include "translator.hpp"
#include "utils.hpp"

namespace lt = libtorrent;
using pt::FileStorageItemModel;

FileStorageItemModel::FileStorageItemModel()
{
    m_iconProvider = new QFileIconProvider();
    m_priorityTexts.insert({ lt::dont_download,    i18n("do_not_download") });
    m_priorityTexts.insert({ lt::default_priority, i18n("normal") });
    m_priorityTexts.insert({ lt::low_priority,     i18n("low") });
    m_priorityTexts.insert({ lt::top_priority,     i18n("maximum") });
}

FileStorageItemModel::~FileStorageItemModel()
{
    delete m_iconProvider;
}

std::vector<int> FileStorageItemModel::fileIndices(QModelIndexList const& indices)
{
    std::function<void(FileNode*, std::vector<int>&)> IterateNodes = [&](FileNode* node, std::vector<int>& result)
    {
        if (node->children.size() > 0)
        {
            for (std::shared_ptr<FileNode> const& child : node->children)
            {
                IterateNodes(child.get(), result);
            }
        }
        else
        {
            result.push_back(node->index);
        }
    };

    std::vector<int> res;

    for (QModelIndex const& idx : indices)
    {
        if (!idx.isValid())
        {
            continue;
        }

        IterateNodes(reinterpret_cast<FileNode*>(idx.internalPointer()), res);
    }

    return res;
}

int FileStorageItemModel::columnCount(QModelIndex const&) const
{
    return Columns::_Max;
}

QVariant FileStorageItemModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    FileNode* item = static_cast<FileNode*>(index.internalPointer());

    switch (role)
    {
    case Qt::DecorationRole:
    {
        switch (index.column())
        {
        case Columns::Name:
        {
            if (item->children.size() > 0)
            {
                return m_iconProvider->icon(QFileIconProvider::Folder);
            }

            QFileInfo f(item->name);
            return m_iconProvider->icon(f);
        }
        }
        break;
    }
    case Qt::DisplayRole:
    {
        switch (index.column())
        {
        case Columns::Name:
            return item->name;
        case Columns::Size:
        {
            if (item->children.size() > 0)
            {
                return "";
            }

            return Utils::ToHumanFileSize(item->size);
        }
        case Columns::Progress:
        {
            if (item->children.size() > 0)
            {
                return -99;
            }

            if (item->progress > 0)
            {
                return static_cast<float>(item->progress) / item->size;
            }
            return 0;
        }
        case Columns::Priority:
        {
            if (item->children.size() > 0)
            {
                return "";
            }

            return m_priorityTexts.at(item->priority);
        }
        }
        break;
    }
    case Qt::TextAlignmentRole:
    {
        switch (index.column())
        {
        case Columns::Size:
        {
            QFlags<Qt::AlignmentFlag> flag;
            flag |= Qt::AlignRight;
            flag |= Qt::AlignVCenter;
            return QVariant(flag);
        }
        }
        break;
    }
    }

    return QVariant();
}

QModelIndex FileStorageItemModel::index(int row, int column, QModelIndex const& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    FileNode* item;

    if (parent.isValid())
    {
        item = static_cast<FileNode*>(parent.internalPointer());
    }
    else
    {
        item = m_root.get();
    }

    if (item->children.size() >= row)
    {
        return createIndex(row, column, item->children.at(row).get());
    }

    return QModelIndex();
}

QVariant FileStorageItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (section)
        {
        case Columns::Name:
            return i18n("name");

        case Columns::Size:
            return i18n("size");

        case Columns::Progress:
            return i18n("progress");

        case Columns::Priority:
            return i18n("priority");
        }

        break;
    }

    case Qt::TextAlignmentRole:
    {
        switch (section)
        {
        case Columns::Size:
            return Qt::AlignRight;
        }
        break;
    }
    }

    return QVariant();
}

QModelIndex FileStorageItemModel::parent(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    FileNode* item = static_cast<FileNode*>(index.internalPointer());

    if (!item->parent)
    {
        return QModelIndex();
    }

    FileNode* parent = item->parent.get();

    if (parent == m_root.get())
    {
        return QModelIndex();
    }

    auto res = std::find_if(
        parent->parent->children.begin(),
        parent->parent->children.end(),
        [=](std::shared_ptr<FileNode> node)
        {
            return node->name == parent->name;
        });

    if (res == parent->parent->children.end())
    {
        return createIndex(0, 0, parent);
    }

    auto distance = std::distance(
        parent->parent->children.begin(),
        res);

    return createIndex(distance, 0, parent);
}

void FileStorageItemModel::rebuildTree(std::shared_ptr<const lt::torrent_info> ti)
{
    this->beginResetModel();

    m_map.clear();
    m_root = std::make_shared<FileNode>();

    lt::file_storage const& files = ti->files();

    for (int i = 0; i < files.num_files(); i++)
    {
        libtorrent::file_index_t idx { i };

        auto currentNode = m_root;

        QString path = QString::fromStdString(files.file_path(idx));
        QStringList parts = path.split("\\");

        for (int j = 0; j < parts.size(); j++)
        {
            QString const& part = parts.at(j);

            // Find child in current
            auto res = std::find_if(
                currentNode->children.begin(),
                currentNode->children.end(),
                [=](std::shared_ptr<FileNode> n)
                {
                    return n->name == part;
                });

            // If we did not match anything - i.e if this is a new node
            if (res == currentNode->children.end())
            {
                auto node = std::make_shared<FileNode>();
                node->name = part;
                node->parent = currentNode;

                if (j == parts.size() - 1)
                {
                    node->index = i;
                    node->size = files.file_size(idx);

                    m_map.insert({ i, node });
                }

                currentNode->children.push_back(node);
                currentNode = node;
            }
            else
            {
                currentNode = *res;
            }
        }
    }

    this->endResetModel();
}

int FileStorageItemModel::rowCount(QModelIndex const& parent) const
{
    if (parent.column() > 0 || !m_root)
    {
        return 0;
    }

    FileNode* item;

    if (parent.isValid())
    {
        item = static_cast<FileNode*>(parent.internalPointer());
    }
    else
    {
        item = m_root.get();
    }

    return static_cast<int>(item->children.size());
}

void FileStorageItemModel::setPriorities(std::vector<lt::download_priority_t> const& priorities)
{
    for (int i = 0; i < priorities.size(); i++)
    {
        auto const& node = m_map.at(i);

        if (node->priority == priorities.at(i))
        {
            continue;
        }

        node->priority = priorities.at(i);
    }
    
    emit dataChanged(QModelIndex{}, QModelIndex{});
}

void FileStorageItemModel::setProgress(std::vector<int64_t> const& progress)
{
    for (int i = 0; i < progress.size(); i++)
    {
        auto const& node = m_map.at(i);

        if (node->progress == progress.at(i))
        {
            continue;
        }

        node->progress = progress.at(i);
    }
    
    emit dataChanged(QModelIndex{}, QModelIndex{});
}
