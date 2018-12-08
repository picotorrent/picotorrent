#pragma once

#include <QDialog>

#include <memory>
#include <vector>

#include <libtorrent/fwd.hpp>

class QActionGroup;
class QComboBox;
class QLabel;
class QMenu;
class QPoint;
class QTreeView;

namespace pt
{
    class FileStorageItemModel;

    class AddTorrentDialog : public QDialog
    {
    public:
        AddTorrentDialog(QWidget* parent, std::vector<libtorrent::add_torrent_params> const& params);

    private:
        enum FilePriority
        {
            Skip,
            Low,
            Normal,
            Max
        };

        void onSetTorrentFilePriorities(QAction* action);
        void onTorrentFileContextMenu(QPoint const& point);
        void onTorrentIndexChanged(int index);

        std::vector<libtorrent::add_torrent_params> m_params;

        QComboBox* m_paramsList;
        QLabel* m_torrentName;
        QLabel* m_torrentSize;
        QLabel* m_torrentInfoHash;
        QLabel* m_torrentComment;
        QTreeView* m_torrentFiles;

        QMenu* m_torrentContextMenu;

        FileStorageItemModel* m_filesModel;
    };
}
