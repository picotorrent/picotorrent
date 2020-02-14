#pragma once

#include <QDialog>

#include <memory>
#include <vector>

#include <libtorrent/fwd.hpp>

class QActionGroup;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPoint;
class QPushButton;
class QTreeView;

namespace pt
{
    class Database;
    class FileStorageItemModel;

    class AddTorrentDialog : public QDialog
    {
    public:
        AddTorrentDialog(std::vector<libtorrent::add_torrent_params>& params, std::shared_ptr<Database> db, QWidget* parent);

        std::vector<libtorrent::add_torrent_params> getParams();

    public slots:
        void refreshMetadata(std::shared_ptr<libtorrent::torrent_info>* ti);

    private:
        QString getDisplayComment(libtorrent::add_torrent_params const& param);
        QString getDisplayHash(libtorrent::add_torrent_params const& param);
        QString getDisplayName(libtorrent::add_torrent_params const& param);
        QString getDisplaySize(libtorrent::add_torrent_params const& param);

        void onSetTorrentFilePriorities(QAction* action);
        void onTorrentFileContextMenu(QPoint const& point);
        void onTorrentIndexChanged(int index);
        void onTorrentSavePathBrowse();
        void onTorrentSavePathChanged(QString const& text);
        void onTorrentSequentialDownloadChanged(int state);
        void onTorrentStartChanged(int state);

        void updateHistory();

        std::vector<libtorrent::add_torrent_params> m_params;
        std::shared_ptr<Database> m_db;

        QComboBox* m_paramsList;
        QLabel* m_torrentName;
        QLabel* m_torrentSize;
        QLabel* m_torrentInfoHash;
        QLabel* m_torrentComment;
        QComboBox* m_torrentSavePath;
        QPushButton* m_torrentSavePathBrowse;
        QCheckBox* m_torrentSequentialDownload;
        QCheckBox* m_torrentStart;
        QTreeView* m_torrentFiles;
        QDialogButtonBox* m_buttons;

        QMenu* m_torrentContextMenu;

        FileStorageItemModel* m_filesModel;
    };
}
