#include "mainwindow.hpp"

#include <filesystem>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>

#include <picotorrent.hpp>

#include <QDir>
#include <QFileDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPluginLoader>

#include "addtorrentdialog.hpp"
#include "configuration.hpp"
#include "environment.hpp"
#include "torrentdetailswidget.hpp"
#include "torrentlistmodel.hpp"
#include "torrentlistwidget.hpp"

namespace fs = std::experimental::filesystem;
using pt::MainWindow;

MainWindow::MainWindow(std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Configuration> cfg)
    : m_env(env),
    m_cfg(cfg)
{
    m_torrentDetails = new TorrentDetailsWidget();
    m_torrentList = new TorrentListWidget();
    m_torrentList->setModel(new TorrentListModel());

    m_splitter = new QSplitter();
    m_splitter->addWidget(m_torrentList);
    m_splitter->addWidget(m_torrentDetails);
    m_splitter->setCollapsible(0, false);
    m_splitter->setOrientation(Qt::Vertical);

    /* Create actions */
    m_fileAddTorrent = new QAction("&Add torrent", this);
    m_fileAddMagnetLinks = new QAction("Add &magnet links", this);
    m_fileExit = new QAction("E&xit", this);
    m_viewPreferences = new QAction("&Preferences", this);
    m_helpAbout = new QAction("&About", this);

    connect(m_fileAddTorrent, &QAction::triggered, this, &MainWindow::onFileAddTorrent);
    connect(m_fileExit, &QAction::triggered, this, &MainWindow::onFileExit);

    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_fileAddTorrent);
    fileMenu->addAction(m_fileAddMagnetLinks);
    fileMenu->addSeparator();
    fileMenu->addAction(m_fileExit);

    auto viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(m_viewPreferences);

    auto helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_helpAbout);

    this->setCentralWidget(m_splitter);
    this->setMinimumSize(350, 200);
    this->setWindowIcon(QIcon(":res/app.ico"));  
    this->setWindowTitle("PicoTorrent");

    fs::path appDir = QApplication::applicationDirPath().toStdWString();
    fs::path pluginsDir = appDir / "plugins";

    for (auto& p : fs::directory_iterator(pluginsDir))
    {
        if (!fs::is_directory(p.path()))
        {
            continue;
        }

        fs::path pluginName = p.path().filename();

        fs::path pluginFile = p.path().filename();
        pluginFile.replace_extension(".dll");

        fs::path pluginFilePath = p.path() / pluginFile;

        if (!fs::is_regular_file(pluginFilePath))
        {
            continue;
        }

        QPluginLoader pluginLoader(QString::fromStdWString(pluginFilePath.wstring()));
        QObject* plugin = pluginLoader.instance();

        if (plugin)
        {
            IPlugin* pl = qobject_cast<IPlugin*>(plugin);

            if (pl)
            {
                m_plugins.push_back(pl);
                pl->load(this);
            }
        }
    }
}

pt::ITorrentDetailsWidget* MainWindow::torrentDetails()
{
    return m_torrentDetails;
}

pt::ITorrentListWidget* MainWindow::torrentList()
{
    return m_torrentList;
}

/* Actions */
void MainWindow::onFileAddTorrent()
{
    QStringList filters;
    filters << "Torrent files (*.torrent)"
            << "All files (*)";

    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::ExistingFiles);
    dlg.setNameFilters(filters);
    
    if (dlg.exec())
    {
        std::vector<lt::add_torrent_params> params;

        for (QString const& fileName : dlg.selectedFiles())
        {
            fs::path p = fs::absolute(fileName.toStdString());

            lt::error_code ec;
            lt::add_torrent_params param;

            param.save_path = m_cfg->getString("default_save_path");
            param.ti = std::make_shared<lt::torrent_info>(p.string(), ec);

            if (ec)
            {
                // TODO(error log)
                continue;
            }

            params.push_back(param);
        }

        AddTorrentDialog addDlg(this, params);
        addDlg.exec();
    }
}

void MainWindow::onFileExit()
{
}
