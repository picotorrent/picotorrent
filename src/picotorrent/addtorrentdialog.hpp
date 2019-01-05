#pragma once

#include <QDialog>

#include <memory>
#include <vector>

#include <libtorrent/fwd.hpp>

class QActionGroup;
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPoint;
class QPushButton;
class QTreeView;

namespace pt
{
    class FileStorageItemModel;

    class AddTorrentDialog : public QDialog
    {
    public:
        AddTorrentDialog(QWidget* parent, std::vector<libtorrent::add_torrent_params>& params);

    private:
        void onSetTorrentFilePriorities(QAction* action);
        void onTorrentFileContextMenu(QPoint const& point);
        void onTorrentIndexChanged(int index);
        void onTorrentSavePathBrowse();
        void onTorrentSavePathChanged(QString const& text);
        void onTorrentSequentialDownloadChanged(int state);
        void onTorrentStartChanged(int state);

        std::vector<libtorrent::add_torrent_params>& m_params;

        QComboBox* m_paramsList;
        QLabel* m_torrentName;
        QLabel* m_torrentSize;
        QLabel* m_torrentInfoHash;
        QLabel* m_torrentComment;
        QLineEdit* m_torrentSavePath;
        QPushButton* m_torrentSavePathBrowse;
        QCheckBox* m_torrentSequentialDownload;
        QCheckBox* m_torrentStart;
        QTreeView* m_torrentFiles;
        QPushButton* m_ok;
        QPushButton* m_cancel;

        QMenu* m_torrentContextMenu;

        FileStorageItemModel* m_filesModel;
    };
}
