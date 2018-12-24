#include "mainwindow.hpp"

#include <filesystem>
#include <vector>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/write_resume_data.hpp>

#include <picotorrent.hpp>

#include <QAction>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPluginLoader>
#include <QSplitter>
#include <QTimer>

#include "addtorrentdialog.hpp"
#include "configuration.hpp"
#include "database.hpp"
#include "environment.hpp"
#include "sessionloader.hpp"
#include "sessionstate.hpp"
#include "sessionunloader.hpp"
#include "torrentdetailswidget.hpp"
#include "torrentlistmodel.hpp"
#include "torrentlistwidget.hpp"

namespace fs = std::experimental::filesystem;
using pt::MainWindow;

MainWindow::MainWindow(std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Database> db, std::shared_ptr<pt::Configuration> cfg)
    : m_env(env),
    m_db(db),
    m_cfg(cfg)
{
    m_sessionState = SessionLoader::load(db, cfg);
    m_sessionState->session->set_alert_notify([this]()
    {
        QMetaObject::invokeMethod(this, "readAlerts", Qt::QueuedConnection);
    });

    m_torrentDetails = new TorrentDetailsWidget();

    m_torrentListModel = new TorrentListModel();
    m_torrentList = new TorrentListWidget(this, m_torrentListModel, m_db);

    m_splitter = new QSplitter();
    m_splitter->addWidget(m_torrentList);
    m_splitter->addWidget(m_torrentDetails);
    m_splitter->setCollapsible(0, false);
    m_splitter->setOrientation(Qt::Vertical);

    m_updateTimer = new QTimer(this);

    /* Create actions */
    m_fileAddTorrent = new QAction("&Add torrent", this);
    m_fileAddMagnetLinks = new QAction("Add &magnet links", this);
    m_fileExit = new QAction("E&xit", this);
    m_viewPreferences = new QAction("&Preferences", this);
    m_helpAbout = new QAction("&About", this);

    connect(m_fileAddTorrent, &QAction::triggered, this, &MainWindow::onFileAddTorrent);
    connect(m_fileExit, &QAction::triggered, this, &MainWindow::onFileExit);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::postUpdates);

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

    fs::path pluginsDir = env->getApplicationDataPath() / "plugins";

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

    m_updateTimer->start(1000);
}

MainWindow::~MainWindow()
{
    m_updateTimer->stop();
    m_sessionState->session->set_alert_notify([]{});

    SessionUnloader::unload(m_sessionState, m_db);
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
        
        if (addDlg.exec())
        {
            for (lt::add_torrent_params& p : params)
            {
                m_sessionState->session->async_add_torrent(p);
            }
        }
    }
}

void MainWindow::onFileExit()
{
}

void MainWindow::readAlerts()
{
    std::vector<lt::alert*> alerts;
    m_sessionState->session->pop_alerts(&alerts);

    for (lt::alert* alert : alerts)
    {
        qDebug() << alert->message().c_str();

        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (ata->error)
            {
                // TODO(log)
                continue;
            }

            std::stringstream ss;
            ss << ata->handle.info_hash();
            std::string ih = ss.str();

            auto stmt = m_db->statement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->bind(1, ih);
            stmt->bind(2, static_cast<int>(ata->handle.status().queue_position));
            stmt->execute();

            ata->handle.save_resume_data(
                lt::torrent_handle::flush_disk_cache
                | lt::torrent_handle::save_info_dict);

            m_torrentListModel->addTorrent(ata->handle.status());

            break;
        }

        case lt::save_resume_data_alert::alert_type:
        {
            lt::save_resume_data_alert* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);
            std::vector<char> buffer = lt::write_resume_data_buf(srda->params);

            std::stringstream ss;
            ss << srda->handle.info_hash();
            std::string ih = ss.str();

            // Store the data
            auto stmt = m_db->statement("REPLACE INTO torrent_resume_data (info_hash, resume_data) VALUES (?, ?);");
            stmt->bind(1, ih);
            stmt->bind(2, buffer);
            stmt->execute();

            break;
        }

        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

            for (lt::torrent_status const& status : sua->status)
            {
                m_torrentListModel->updateTorrent(status);
            }

            break;
        }
        }
    }
}

void MainWindow::postUpdates()
{
    m_sessionState->session->post_dht_stats();
    m_sessionState->session->post_session_stats();
    m_sessionState->session->post_torrent_updates();
}
