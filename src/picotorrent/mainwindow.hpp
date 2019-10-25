#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <libtorrent/fwd.hpp>

#include <memory>
#include <vector>

class QAction;
class QActionGroup;
class QDragEnterEvent;
class QDropEvent;
class QItemSelection;
class QLabel;
class QMenu;
class QShortcut;
class QSplitter;
class QTimer;
class QWinTaskbarButton;

namespace pt
{
    class Configuration;
    class Database;
    class Environment;
    class GeoIP;
    class HttpClient;
    class JsEngine;
    class ScriptedTorrentFilter;
    class Session;
    struct SessionState;
    class StatusBar;
    class SystemTrayIcon;
    class TorrentDetailsWidget;
    class TorrentHandle;
    class TorrentListModel;
    class TorrentListWidget;
    class TorrentSortFilterProxyModel;
    struct TorrentStatistics;
    struct UpdateInformation;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(std::shared_ptr<Environment> env, std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg);
        ~MainWindow();

        void handleCommandLine(QStringList const& args);
        void loadScripts();

    protected:
        void changeEvent(QEvent* event) override;
        void closeEvent(QCloseEvent* event) override;
        bool nativeEvent(QByteArray const& eventType, void* message, long* result) override;
        void showEvent(QShowEvent* event) override;

    private slots:
        void addTorrentFilter(ScriptedTorrentFilter* filter);
        void checkDiskSpace(TorrentHandle* torrent);
        void checkForUpdates(bool force = false);
        void onFileAddMagnetLinks();
        void onFileAddTorrent();
        void onHelpAbout();
        void onTorrentContextMenu(QPoint const& point);
        void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
        void onViewPreferences();
        void setTorrentFilter(QAction* action);
        void showTorrentFinishedNotification(TorrentHandle* torrent);
        void showUpdateDialog(UpdateInformation* info);
        void updateTaskbarButton(TorrentStatistics* stats);

    private:
        void addTorrents(std::vector<libtorrent::add_torrent_params>& params);
        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
        void parseMagnetLinks(std::vector<libtorrent::add_torrent_params>& params, QStringList const& magnetLinks);
        void parseTorrentFiles(std::vector<libtorrent::add_torrent_params>& params, QStringList const& files);
        void showHideDetailsPanel(bool show);
        void showHideStatusBar(bool show);

        std::shared_ptr<Environment> m_env;
        std::shared_ptr<Database> m_db;
        std::shared_ptr<Configuration> m_cfg;
        std::shared_ptr<SessionState> m_sessionState;

        QAction* m_fileAddTorrent;
        QAction* m_fileAddMagnetLinks;
        QAction* m_fileExit;
        QAction* m_viewFiltersNone;
        QAction* m_viewPreferences;
        QAction* m_viewDetailsPanel;
        QAction* m_viewStatusBar;
        QAction* m_helpCheckForUpdates;
        QAction* m_helpAbout;

        // Shortcuts
        QShortcut* m_shortcutOpenTorrent;
        QShortcut* m_shortcutRemoveSelectedTorrent;

        QActionGroup* m_filtersGroup;
        QMenu* m_filtersMenu;

        QList<TorrentHandle*> m_selectedTorrents;

        QSplitter* m_splitter;
        QWinTaskbarButton* m_taskbarButton;

        GeoIP* m_geo;
        JsEngine* m_jsEngine;
        Session* m_session;
        SystemTrayIcon* m_trayIcon;
        StatusBar* m_statusBar;
        TorrentDetailsWidget* m_torrentDetails;
        TorrentListModel* m_torrentListModel;
        TorrentListWidget* m_torrentList;
        TorrentSortFilterProxyModel* m_torrentSortFilterModel;
        int m_torrentsCount;
    };
}
