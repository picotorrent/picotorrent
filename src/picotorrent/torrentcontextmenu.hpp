#pragma once

#include <QMenu>

#include <memory>

class QAction;
class QWidget;

namespace pt
{
    struct SessionState;
    class Translator;

    class TorrentContextMenu : public QMenu
    {
    public:
        TorrentContextMenu(QWidget* parent,
            std::shared_ptr<SessionState> state);

    private:
        void copyInfoHash();
        void openExplorer();
        void queueUp();
        void queueDown();
        void queueTop();
        void queueBottom();
        void remove();
        void removeFiles();

        QWidget* m_parent;
        std::shared_ptr<SessionState> m_state;

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
