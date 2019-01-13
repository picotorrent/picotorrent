#pragma once

#include <QMainWindow>

#include <memory>
#include <vector>

class QAction;
class QItemSelection;
class QLabel;
class QSplitter;
class QTimer;

namespace pt
{
    class Configuration;
    class Database;
    class Environment;
    class GeoIP;
    class PreferencesDialog;
    struct SessionState;
    class StatusBar;
    class SystemTrayIcon;
    class TorrentContextMenu;
    class TorrentDetailsWidget;
    class TorrentListModel;
    class TorrentListWidget;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(std::shared_ptr<Environment> env, std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg);
        ~MainWindow();

    protected:
        bool nativeEvent(QByteArray const& eventType, void* message, long* result) override;

    private slots:
        void readAlerts();

    private:
        void onFileAddTorrent();
        void onFileExit();
        void onHelpAbout();
        void onTorrentSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected);
        void onTorrentContextMenu(QPoint const& point);
        void onViewPreferences();

        void postUpdates();
        void reloadConfig();

        std::shared_ptr<Environment> m_env;
        std::shared_ptr<Database> m_db;
        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<GeoIP> m_geo;
        std::shared_ptr<SessionState> m_sessionState;

        // Menus
        TorrentContextMenu* m_torrentContextMenu;

        QAction* m_fileAddTorrent;
        QAction* m_fileAddMagnetLinks;
        QAction* m_fileExit;
        QAction* m_viewPreferences;
        QAction* m_helpAbout;

        // Dialogs
        PreferencesDialog* m_preferencesDialog;

        QSplitter* m_splitter;
        QTimer* m_updateTimer;

        SystemTrayIcon* m_trayIcon;
        StatusBar* m_statusBar;
        TorrentDetailsWidget* m_torrentDetails;
        TorrentListModel* m_torrentListModel;
        TorrentListWidget* m_torrentList;
    };
}
