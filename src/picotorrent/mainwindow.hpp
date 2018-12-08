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
    class TorrentDetailsWidget;
    class TorrentListWidget;

    class MainWindow : public QMainWindow, IPluginHost
    {
    public:
        MainWindow(QApplication& app);

        ITorrentDetailsWidget* torrentDetails() override;
        ITorrentListWidget* torrentList() override;

    private:
        void onFileAddTorrent();
        void onFileExit();

        QApplication& m_app;

        std::vector<IPlugin*> m_plugins;

        QAction* m_fileAddTorrent;
        QAction* m_fileExit;

        QSplitter* m_splitter;
        TorrentDetailsWidget* m_torrentDetails;
        TorrentListWidget* m_torrentList;
    };
}
