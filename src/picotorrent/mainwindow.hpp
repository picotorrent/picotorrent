#pragma once

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <libtorrent/fwd.hpp>

#include <memory>
#include <vector>

class QAction;
class QItemSelection;
class QLabel;
class QSplitter;
class QTimer;
class QWinTaskbarButton;

namespace pt
{
    class Configuration;
    class Database;
    class Environment;
    class GeoIP;
    class Session;
    struct SessionState;
    class StatusBar;
    class SystemTrayIcon;
    class TorrentDetailsWidget;
    class TorrentHandle;
    class TorrentListModel;
    class TorrentListWidget;
    struct TorrentStatistics;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(std::shared_ptr<Environment> env, std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg);
        ~MainWindow();

        void handleCommandLine(QStringList const& args);

    protected:
        void changeEvent(QEvent* event) override;
        void closeEvent(QCloseEvent* event) override;
        bool nativeEvent(QByteArray const& eventType, void* message, long* result) override;
        void showEvent(QShowEvent* event) override;

    private slots:
        void onFileAddMagnetLinks();
        void onFileAddTorrent();
        void onHelpAbout();
        void onTorrentContextMenu(QPoint const& point);
        void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
        void onViewPreferences();
        void updateTaskbarButton(TorrentStatistics* stats);

    private:
        void addTorrents(std::vector<libtorrent::add_torrent_params>& params);
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
        QAction* m_viewPreferences;
        QAction* m_viewDetailsPanel;
        QAction* m_viewStatusBar;
        QAction* m_helpAbout;

        QList<TorrentHandle*> m_selectedTorrents;

        QSplitter* m_splitter;
        QWinTaskbarButton* m_taskbarButton;

        GeoIP* m_geo;
        Session* m_session;
        SystemTrayIcon* m_trayIcon;
        StatusBar* m_statusBar;
        TorrentDetailsWidget* m_torrentDetails;
        TorrentListModel* m_torrentListModel;
        TorrentListWidget* m_torrentList;
        int m_torrentsCount;
    };
}
