#include "mainwindow.hpp"

#include <filesystem>
#include <vector>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>

#include <picotorrent/core/configuration.hpp>
#include <picotorrent/core/database.hpp>
#include <picotorrent/core/environment.hpp>
#include <picotorrent/geoip/geoip.hpp>

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
#include "preferencesdialog.hpp"
#include "session.hpp"
#include "sessionstate.hpp"
#include "statusbar.hpp"
#include "systemtrayicon.hpp"
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
    m_cfg(cfg)
{
    m_session = new Session(this, db, cfg);
    m_geo = new GeoIP(this, m_env, m_cfg);

    m_torrentDetails = new TorrentDetailsWidget(this, m_sessionState, m_geo);

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

    /* Create actions */
    m_fileAddTorrent = new QAction(i18n("amp_add_torrent"), this);
    m_fileAddMagnetLinks = new QAction(i18n("amp_add_magnet_link_s"), this);
    m_fileExit = new QAction(i18n("amp_exit"), this);
    m_viewPreferences = new QAction(i18n("amp_preferences"), this);
    m_helpAbout = new QAction(i18n("amp_about"), this);

    // Session signals
    QObject::connect(m_session,          &Session::torrentAdded,
                     m_torrentListModel, &TorrentListModel::addTorrent);

    QObject::connect(m_session,          &Session::torrentRemoved,
                     m_torrentListModel, &TorrentListModel::removeTorrent);

    QObject::connect(m_session,          &Session::torrentUpdated,
                     m_torrentListModel, &TorrentListModel::updateTorrent);

    // Main menu signals
    QObject::connect(m_fileAddTorrent,  &QAction::triggered,
                     this,              &MainWindow::onFileAddTorrent);

    QObject::connect(m_fileExit,        &QAction::triggered,
                     this,              &MainWindow::onFileExit);

    QObject::connect(m_viewPreferences, &QAction::triggered,
                     this,              &MainWindow::onViewPreferences);

    QObject::connect(m_helpAbout,       &QAction::triggered,
                     this,              &MainWindow::onHelpAbout);

    // Torrent list signals
    QObject::connect(m_torrentList,     &TorrentListWidget::torrentsSelected,
                     m_torrentDetails,  &TorrentDetailsWidget::update);

    connect(
        m_torrentList,
        &QTreeView::customContextMenuRequested,
        this,
        &MainWindow::onTorrentContextMenu);

    // GeoIP signals
    connect(m_geo, &GeoIP::updateRequired,
            m_geo, &GeoIP::update);

    // System tray
    connect(m_trayIcon, &SystemTrayIcon::addTorrentRequested, this, &MainWindow::onFileAddTorrent);

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

    if (m_cfg->getBool("geoip.enabled"))
    {
        m_geo->load();
    }

    m_statusBar->updateTorrentCount(0);
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
                m_session->addTorrent(p);
            }
        }
    }
}

void MainWindow::onFileExit()
{
}

void MainWindow::onHelpAbout()
{
    auto dlg = new AboutDialog(this);
    dlg->open();

    QObject::connect(dlg, &QDialog::finished,
                     dlg, &QDialog::deleteLater);
}

void MainWindow::onTorrentContextMenu(QPoint const& point)
{
    QModelIndex idx = m_torrentList->indexAt(point);

    if (idx.isValid())
    {
        auto menu = new TorrentContextMenu(this, m_sessionState);
        menu->popup(m_torrentList->viewport()->mapToGlobal(point));

        QObject::connect(menu, &QMenu::aboutToHide,
                         menu, &QMenu::deleteLater);
    }
}

void MainWindow::onViewPreferences()
{
    auto dlg = new PreferencesDialog(this, m_cfg);
    dlg->open();

    QObject::connect(dlg,       &QDialog::accepted,
                     m_session, &Session::reloadSettings);

    QObject::connect(dlg,       &QDialog::finished,
                     dlg,       &QDialog::deleteLater);
}
