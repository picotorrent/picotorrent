#pragma once

#include <picotorrent.hpp>

#include <QMainWindow>

#include <memory>
#include <vector>

class QAction;
class QSplitter;
class QTimer;

namespace pt
{
    class Configuration;
    class Database;
    class Environment;
    struct SessionState;
    class SystemTrayIcon;
    class TorrentDetailsWidget;
    class TorrentListModel;
    class TorrentListWidget;

    class MainWindow : public QMainWindow, IPluginHost
    {
        Q_OBJECT

    public:
        MainWindow(std::shared_ptr<Environment> env, std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg);

        ITorrentDetailsWidget* torrentDetails() override;
        ITorrentListWidget* torrentList() override;

    private slots:
        void readAlerts();

    private:
        void onFileAddTorrent();
        void onFileExit();
        void onHelpAbout();
        void onViewPreferences();

        void postUpdates();

        std::shared_ptr<Environment> m_env;
        std::shared_ptr<Database> m_db;
        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<SessionState> m_sessionState;

        std::vector<IPlugin*> m_plugins;

        QAction* m_fileAddTorrent;
        QAction* m_fileAddMagnetLinks;
        QAction* m_fileExit;
        QAction* m_viewPreferences;
        QAction* m_helpAbout;

        QSplitter* m_splitter;
        QTimer* m_updateTimer;

        SystemTrayIcon* m_trayIcon;
        TorrentDetailsWidget* m_torrentDetails;
        TorrentListModel* m_torrentListModel;
        TorrentListWidget* m_torrentList;
    };
}
