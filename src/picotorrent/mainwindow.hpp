#pragma once

#include <picotorrent.hpp>

#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QSplitter>

#include <memory>
#include <vector>

namespace pt
{
    class Configuration;
    class Environment;
    class TorrentDetailsWidget;
    class TorrentListWidget;

    class MainWindow : public QMainWindow, IPluginHost
    {
    public:
        MainWindow(std::shared_ptr<Environment> env, std::shared_ptr<Configuration> cfg);

        ITorrentDetailsWidget* torrentDetails() override;
        ITorrentListWidget* torrentList() override;

    private:
        void onFileAddTorrent();
        void onFileExit();

        std::shared_ptr<Environment> m_env;
        std::shared_ptr<Configuration> m_cfg;

        std::vector<IPlugin*> m_plugins;

        QAction* m_fileAddTorrent;
        QAction* m_fileAddMagnetLinks;
        QAction* m_fileExit;
        QAction* m_viewPreferences;
        QAction* m_helpAbout;

        QSplitter* m_splitter;
        TorrentDetailsWidget* m_torrentDetails;
        TorrentListWidget* m_torrentList;
    };
}
