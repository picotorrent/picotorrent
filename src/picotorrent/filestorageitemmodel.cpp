#include "filestorageitemmodel.hpp"

#include <libtorrent/file_storage.hpp>
#include <libtorrent/torrent_info.hpp>

#include <QFileIconProvider>
#include <QFileInfo>

#include "utils.hpp"

namespace lt = libtorrent;
using pt::FileStorageItemModel;

FileStorageItemModel::FileStorageItemModel()
{
    m_iconProvider = new QFileIconProvider();
}

FileStorageItemModel::~FileStorageItemModel()
{
    delete m_iconProvider;
}

std::vector<lt::file_index_t> FileStorageItemModel::fileIndices(QModelIndexList const& indices)
{
    std::vector<lt::file_index_t> result;

    return result;
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
        }
        break;
    }
    case Qt::TextAlignmentRole:
    {
        switch (index.column())
        {
        case Columns::Size:
            return Qt::AlignRight;
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
            return QString("Name");

        case Columns::Size:
            return QString("Size");

        case Columns::Priority:
            return QString("Priority");
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

    m_root = std::make_shared<FileNode>();

    lt::file_storage const& files = ti->files();

    for (int i = 0; i < files.num_files(); i++)
    {
        libtorrent::file_index_t idx { i };

        auto currentNode = m_root;

        QString path = QString::fromStdString(files.file_path(idx));
        QStringList parts = path.split("\\");

        for (QString const& part : parts)
        {
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
                node->index = idx;
                node->name = part;
                node->parent = currentNode;
                node->size = files.file_size(idx);

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
