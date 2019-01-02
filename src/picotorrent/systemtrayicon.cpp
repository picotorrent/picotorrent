#include "systemtrayicon.hpp"

#include <QAction>
#include <QMenu>

using pt::SystemTrayIcon;

SystemTrayIcon::SystemTrayIcon(QObject* parent)
    : QSystemTrayIcon(parent)
{
    m_menu = new QMenu();
    m_addTorrentAction = new QAction("Add torrent");
    m_addMagnetLinkAction = new QAction("Add magnet link");
    m_preferencesAction = new QAction("Preferences");
    m_exitAction = new QAction("Exit");

    m_menu->addAction(m_addTorrentAction);
    m_menu->addAction(m_addMagnetLinkAction);
    m_menu->addSeparator();
    m_menu->addAction(m_preferencesAction);
    m_menu->addSeparator();
    m_menu->addAction(m_exitAction);

    connect(m_addTorrentAction, &QAction::triggered, this, &SystemTrayIcon::onAddTorrent);

    this->setContextMenu(m_menu);
    this->setIcon(QIcon(":res/app.ico"));
}

void SystemTrayIcon::onAddTorrent()
{
    emit addTorrentInvoked();
}
