#pragma once

#include <QSystemTrayIcon>

class QAction;
class QMenu;
class QObject;

namespace pt
{
    class SystemTrayIcon : public QSystemTrayIcon
    {
        Q_OBJECT

    public:
        SystemTrayIcon(QObject* parent);

    signals:
        void addTorrentInvoked();

    private:
        void onAddTorrent();

        QMenu* m_menu;
        QAction* m_addTorrentAction;
        QAction* m_addMagnetLinkAction;
        QAction* m_preferencesAction;
        QAction* m_exitAction;
    };
}