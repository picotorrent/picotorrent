#include "mainwindow.hpp"

#include <filesystem>
#include <vector>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/write_resume_data.hpp>

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
#include <QStatusBar>
#include <QTimer>

#include "aboutdialog.hpp"
#include "addtorrentdialog.hpp"
#include "configuration.hpp"
#include "database.hpp"
#include "environment.hpp"
#include "http/httpclient.hpp"
#include "preferencesdialog.hpp"
#include "sessionloader.hpp"
#include "sessionstate.hpp"
#include "statusbar.hpp"
#include "systemtrayicon.hpp"
#include "sessionunloader.hpp"
#include "torrentcontextmenu.hpp"
#include "torrentdetails/torrentdetailswidget.hpp"
#include "torrentlistmodel.hpp"
#include "torrentlistwidget.hpp"
#include "translator.hpp"

namespace fs = std::experimental::filesystem;
using pt::MainWindow;

MainWindow::MainWindow(std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Database> db, std::shared_ptr<pt::Configuration> cfg)
    : m_env(env),
    m_db(db),
    m_cfg(cfg),
    m_preferencesDialog(nullptr)
{
    m_sessionState = SessionLoader::load(db, cfg);
    m_sessionState->session->set_alert_notify([this]()
    {
        QMetaObject::invokeMethod(this, "readAlerts", Qt::QueuedConnection);
    });

    m_torrentContextMenu = new TorrentContextMenu(this, m_sessionState);
    m_torrentDetails = new TorrentDetailsWidget(this, m_sessionState);

    m_torrentListModel = new TorrentListModel();
    m_torrentList = new TorrentListWidget(this, m_torrentListModel, m_db);
    m_torrentList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    m_splitter = new QSplitter();
    m_splitter->addWidget(m_torrentList);
    m_splitter->addWidget(m_torrentDetails);
    m_splitter->setCollapsible(0, false);
    m_splitter->setOrientation(Qt::Vertical);

    m_statusBar = new StatusBar(this);
    m_trayIcon = new SystemTrayIcon(this);
    m_updateTimer = new QTimer(this);

    /* Create actions */
    m_fileAddTorrent = new QAction(i18n("amp_add_torrent"), this);
    m_fileAddMagnetLinks = new QAction(i18n("amp_add_magnet_link_s"), this);
    m_fileExit = new QAction(i18n("amp_exit"), this);
    m_viewPreferences = new QAction(i18n("amp_preferences"), this);
    m_helpAbout = new QAction(i18n("amp_about"), this);

    connect(m_fileAddTorrent, &QAction::triggered, this, &MainWindow::onFileAddTorrent);
    connect(m_fileExit, &QAction::triggered, this, &MainWindow::onFileExit);
    connect(m_viewPreferences, &QAction::triggered, this, &MainWindow::onViewPreferences);
    connect(m_helpAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::postUpdates);

    connect(
        m_torrentList->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &MainWindow::onTorrentSelectionChanged);

    connect(
        m_torrentList,
        &QTreeView::customContextMenuRequested,
        this,
        &MainWindow::onTorrentContextMenu);

    // System tray
    connect(m_trayIcon, &SystemTrayIcon::addTorrentInvoked, this, &MainWindow::onFileAddTorrent);

    auto fileMenu = menuBar()->addMenu(i18n("amp_file"));
    fileMenu->addAction(m_fileAddTorrent);
    fileMenu->addAction(m_fileAddMagnetLinks);
    fileMenu->addSeparator();
    fileMenu->addAction(m_fileExit);

    auto viewMenu = menuBar()->addMenu(i18n("amp_view"));
    viewMenu->addAction(m_viewPreferences);

    auto helpMenu = menuBar()->addMenu(i18n("amp_help"));
    helpMenu->addAction(m_helpAbout);

    this->setCentralWidget(m_splitter);
    this->setMinimumSize(400, 300);
    this->setStatusBar(m_statusBar);
    this->setWindowIcon(QIcon(":res/app.ico"));  
    this->setWindowTitle("PicoTorrent");

    m_statusBar->updateTorrentCount(0);
    m_updateTimer->start(1000);
}

MainWindow::~MainWindow()
{
    m_updateTimer->stop();
    m_sessionState->session->set_alert_notify([]{});

    SessionUnloader::unload(m_sessionState, m_db);
}

bool MainWindow::nativeEvent(QByteArray const& eventType, void* message, long* result)
{
    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_COPYDATA)
    {
        COPYDATASTRUCT* cds = reinterpret_cast<COPYDATASTRUCT*>(msg->lParam);
        LPTSTR str = reinterpret_cast<LPTSTR>(cds->lpData);
        QString cmd = QString::fromWCharArray(str, cds->cbData);

        // TODO(handle)

        return true;
    }

    return false;
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

void MainWindow::onHelpAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::onTorrentSelectionChanged(QItemSelection const& selected, QItemSelection const& deselected)
{
    m_sessionState->selectedTorrents.clear();

    m_torrentListModel->appendInfoHashes(
        selected.indexes(),
        m_sessionState->selectedTorrents);

    m_torrentDetails->clear();

    if (m_sessionState->selectedTorrents.empty())
    {
        return;
    }

    m_torrentDetails->refresh();
}

void MainWindow::onTorrentContextMenu(QPoint const& point)
{
    QModelIndex idx = m_torrentList->indexAt(point);

    if (idx.isValid())
    {
        m_torrentContextMenu->popup(m_torrentList->viewport()->mapToGlobal(point));
    }
}

void MainWindow::onViewPreferences()
{
    if (m_preferencesDialog == nullptr)
    {
        m_preferencesDialog = new PreferencesDialog(
            this,
            m_cfg);

        connect(
            m_preferencesDialog,
            &QDialog::accepted,
            this,
            &MainWindow::reloadConfig);
    }

    m_preferencesDialog->load();
    m_preferencesDialog->open();
}

void MainWindow::readAlerts()
{
    std::vector<lt::alert*> alerts;
    m_sessionState->session->pop_alerts(&alerts);

    for (lt::alert* alert : alerts)
    {
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

            lt::torrent_status ts = ata->handle.status();

            auto stmt = m_db->statement("REPLACE INTO torrent (info_hash, queue_position) VALUES (?, ?)");
            stmt->bind(1, ih);
            stmt->bind(2, static_cast<int>(ts.queue_position));
            stmt->execute();

            ata->handle.save_resume_data(
                lt::torrent_handle::flush_disk_cache
                | lt::torrent_handle::save_info_dict);

            m_sessionState->torrents.insert({ ts.info_hash, ts.handle });
            m_statusBar->updateTorrentCount(m_sessionState->torrents.size());
            m_torrentListModel->addTorrent(ts);

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

        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
            lt::span<const int64_t> counters = ssa->counters();
            int idx = -1;

            if (!m_cfg->getBool("enable_dht"))
            {
                m_statusBar->updateDhtNodesCount(-1);
            }
            else if ((idx = lt::find_metric_idx("dht.dht_nodes")) >= 0)
            {
                m_statusBar->updateDhtNodesCount(counters[idx]);
            }

            break;
        }

        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

            for (lt::torrent_status const& status : sua->status)
            {
                m_torrentListModel->updateTorrent(status);

                if (m_sessionState->selectedTorrents.find(status.info_hash) != m_sessionState->selectedTorrents.end())
                {
                    m_torrentDetails->refresh();
                }
            }

            break;
        }

        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);
            m_statusBar->updateTorrentCount(m_sessionState->torrents.size());
            break;
        }
        }
    }
}

void MainWindow::reloadConfig()
{
    printf("");
}

void MainWindow::postUpdates()
{
    m_sessionState->session->post_dht_stats();
    m_sessionState->session->post_session_stats();
    m_sessionState->session->post_torrent_updates();
}
