#pragma once

#include <QMenu>

#include <memory>

class QAction;
class QFileDialog;
class QWidget;

namespace pt
{
    class TorrentHandle;

    class TorrentContextMenu : public QMenu
    {
    public:
        TorrentContextMenu(QWidget* parent, QList<TorrentHandle*> const& torrents);

    private:
        void copyInfoHash();
        void move();
        void openExplorer();
        void removeFiles();

        QWidget* m_parent;
        QList<TorrentHandle*> m_torrents;

        QMenu* m_queueMenu;
        QMenu* m_removeMenu;

        // Actions
        QAction* m_pause;
        QAction* m_resume;
        QAction* m_resumeForce;
        QAction* m_move;
        QAction* m_remove;
        QAction* m_removeFiles;
        QAction* m_queueUp;
        QAction* m_queueDown;
        QAction* m_queueTop;
        QAction* m_queueBottom;
        QAction* m_copyHash;
        QAction* m_openExplorer;
        QAction* m_forceRecheck;
        QAction* m_forceReannounce;
        QAction* m_sequentialDownload;
    };
}
