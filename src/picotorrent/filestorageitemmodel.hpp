#pragma once

#include <map>
#include <memory>

#include <QAbstractItemModel>

#include <libtorrent/download_priority.hpp>
#include <libtorrent/fwd.hpp>
#include <libtorrent/units.hpp>

class QFileIconProvider;

namespace pt
{
    class FileStorageItemModel : public QAbstractItemModel
    {
    public:
        enum Columns
        {
            Name,
            Size,
            Progress,
            Priority,
            _Max
        };

        FileStorageItemModel();
        ~FileStorageItemModel();

        std::vector<int> fileIndices(QModelIndexList const& indices);

        int columnCount(QModelIndex const&) const override;
        QVariant data(QModelIndex const&, int) const override;
        QModelIndex index(int, int, QModelIndex const&) const override;
        QVariant headerData(int, Qt::Orientation, int) const override;
        QModelIndex parent(QModelIndex const&) const override;
        void clearTree();
        void rebuildTree(std::shared_ptr<const libtorrent::torrent_info> ti);
        int rowCount(QModelIndex const&) const override;

        void setPriorities(std::vector<libtorrent::download_priority_t> const& priorities);
        void setProgress(std::vector<int64_t> const& progress);

    private:
        struct FileNode
        {
            std::vector<std::shared_ptr<FileNode>> children;
            std::shared_ptr<FileNode> parent;

            int index;
            QString name;
            libtorrent::download_priority_t priority = libtorrent::default_priority;
            int64_t size;
            int64_t progress;
        };

        QFileIconProvider* m_iconProvider;
        std::shared_ptr<FileNode> m_root;
        std::map<int, std::shared_ptr<FileNode>> m_map;
        std::map<libtorrent::download_priority_t, QString> m_priorityTexts;
    };
}
