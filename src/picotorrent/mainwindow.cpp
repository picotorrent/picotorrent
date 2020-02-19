#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <Windows.h>
#include <CommCtrl.h>

#include <filesystem>
#include <vector>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent_info.hpp>

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QClipboard>
#include <QCommandLineParser>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPluginLoader>
#include <QShortcut>
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>

#include "loguru.hpp"

#include "core/configuration.hpp"
#include "core/database.hpp"
#include "core/environment.hpp"
#include "core/geoip/geoip.hpp"

#include "aboutdialog.hpp"
#include "addtorrentdialog.hpp"
#include "buildinfo.hpp"
#include "scripting/jsengine.hpp"
#include "picojson.hpp"
#include "preferencesdialog.hpp"
#include "scriptedtorrentfilter.hpp"
#include "session.hpp"
#include "sessionstate.hpp"
#include "sessionstatistics.hpp"
#include "statusbar.hpp"
#include "systemtrayicon.hpp"
#include "textinputdialog.hpp"
#include "torrentcontextmenu.hpp"
#include "torrentdetails/torrentdetailswidget.hpp"
#include "torrentlistmodel.hpp"
#include "torrentlistwidget.hpp"
#include "torrentsortfilterproxymodel.hpp"
#include "torrentstatistics.hpp"
#include "torrentstatus.hpp"
#include "translator.hpp"
#include "updatechecker.hpp"
#include "updateinformation.hpp"

namespace fs = std::filesystem;
using pt::MainWindow;

MainWindow::MainWindow(std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Database> db, std::shared_ptr<pt::Configuration> cfg)
    : m_env(env),
    m_db(db),
    m_cfg(cfg),
    m_torrentsCount(0),
    m_taskbarButton(nullptr),
    m_ui(new Ui::MainWindow())
{
    m_ui->setupUi(this);

    m_session                = new Session(this, db, cfg, env);
    m_geo                    = new GeoIP(this, env, cfg);
    m_jsEngine               = new JsEngine(this);
    m_taskbarButton          = new QWinTaskbarButton(this);
    m_trayIcon               = new SystemTrayIcon(this);
    m_torrentListModel       = new TorrentListModel();
    m_torrentSortFilterModel = new TorrentSortFilterProxyModel(this);
    m_torrentSortFilterModel->setSourceModel(m_torrentListModel);

    m_ui->torrentDetails->setGeo(m_geo);

    // Setup torrent list and sort/filter-model
    m_ui->torrentList->load(m_torrentListModel, db);
    m_ui->torrentList->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    // Translate menu and actions
    m_ui->menuFile->setTitle(i18n("amp_file"));
    m_ui->menuFilter->setTitle(i18n("amp_filter"));
    m_ui->menuHelp->setTitle(i18n("amp_help"));
    m_ui->menuView->setTitle(i18n("amp_view"));

    m_ui->actionAbout->setText(i18n("amp_about"));
    m_ui->actionAddMagnetLink->setText(i18n("amp_add_magnet_link_s"));
    m_ui->actionAddTorrent->setText(i18n("amp_add_torrent"));
    m_ui->actionCheckForUpdates->setText(i18n("amp_check_for_update"));
    m_ui->actionDetailsPanel->setText(i18n("amp_details_panel"));
    m_ui->actionExit->setText(i18n("amp_exit"));
    m_ui->actionFilterNone->setText(i18n("amp_none"));
    m_ui->actionPreferences->setText(i18n("amp_preferences"));
    m_ui->actionStatusBar->setText(i18n("amp_status_bar"));

    m_filtersGroup = new QActionGroup(this);
    m_filtersGroup->addAction(m_ui->actionFilterNone);
    m_filtersGroup->setExclusive(true);

    m_ui->actionDetailsPanel->setChecked(m_cfg->getBool("ui.show_details_panel"));
    m_ui->actionStatusBar->setChecked(m_cfg->getBool("ui.show_status_bar"));

    // Shortcuts
    m_shortcutOpenTorrent           = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_O), this);
    m_shortcutRemoveSelectedTorrent = new QShortcut(QKeySequence(Qt::Key_Delete), m_ui->torrentList);

    // Shortcut signals
    QObject::connect(m_shortcutOpenTorrent, &QShortcut::activated,
                     this, &MainWindow::onFileAddTorrent);

    QObject::connect(m_shortcutRemoveSelectedTorrent, &QShortcut::activated,
        [this]()
        {
            for (TorrentHandle* th : m_selectedTorrents)
            {
                th->remove();
            }

            m_selectedTorrents.clear();
        });

    // Session signals
    QObject::connect(m_session,          &Session::sessionStatsUpdated,
                     [this](SessionStatistics* stats)
                     {
                         bool dhtEnabled = m_cfg->getBool("enable_dht");
                         m_ui->statusBar->updateDhtNodesCount(dhtEnabled ? stats->dhtNodes : -1);
                     });

    QObject::connect(m_session,          &Session::torrentAdded,
                     [this](TorrentHandle* torrent)
                     {
                         m_torrentsCount++;
                         m_ui->statusBar->updateTorrentCount(m_torrentsCount);
                         m_torrentListModel->addTorrent(torrent);
                     });

    QObject::connect(m_session,          &Session::torrentFinished,
                     this,               &MainWindow::showTorrentFinishedNotification);

    QObject::connect(m_session,          &Session::torrentRemoved,
                     [this](TorrentHandle* torrent)
                     {
                         m_torrentsCount--;
                         m_ui->statusBar->updateTorrentCount(m_torrentsCount);
                         m_torrentListModel->removeTorrent(torrent);
                     });

    QObject::connect(m_session,          &Session::torrentStatsUpdated,
                     [this](TorrentStatistics* stats)
                     {
                         m_ui->statusBar->updateTransferRates(
                             stats->totalPayloadDownloadRate,
                             stats->totalPayloadUploadRate);
                     });

    QObject::connect(m_session,          &Session::torrentStatsUpdated,
                     this,               &MainWindow::updateTaskbarButton);

    QObject::connect(m_session,          &Session::torrentUpdated,
                     this,               &MainWindow::checkDiskSpace);

    QObject::connect(m_session,          &Session::torrentUpdated,
                     m_torrentListModel, &TorrentListModel::updateTorrent);

    QObject::connect(m_session,          &Session::torrentUpdated,
                     [this](TorrentHandle* torrent)
                     {
                         if (m_selectedTorrents.contains(torrent))
                         {
                             m_ui->torrentDetails->update(m_selectedTorrents);
                         }
                     });

    // Main menu signals
    QObject::connect(m_ui->actionAddTorrent,      &QAction::triggered,
                     this,                        &MainWindow::onFileAddTorrent);

    QObject::connect(m_ui->actionAddMagnetLink,   &QAction::triggered,
                     this,                        &MainWindow::onFileAddMagnetLinks);

    QObject::connect(m_ui->actionExit,            &QAction::triggered,
                     this,                        &MainWindow::close);

    QObject::connect(m_ui->actionPreferences,     &QAction::triggered,
                     this,                        &MainWindow::onViewPreferences);

    QObject::connect(m_ui->actionDetailsPanel,    &QAction::toggled,
                     this,                        &MainWindow::showHideDetailsPanel);

    QObject::connect(m_ui->actionStatusBar,       &QAction::toggled,
                     this,                        &MainWindow::showHideStatusBar);

    QObject::connect(m_ui->actionCheckForUpdates, &QAction::triggered,
                     [this] { this->checkForUpdates(true); });

    QObject::connect(m_ui->actionAbout,           &QAction::triggered,
                     this,                        &MainWindow::onHelpAbout);

    QObject::connect(m_filtersGroup,              &QActionGroup::triggered,
                    this,                         &MainWindow::setTorrentFilter);

    // Torrent list signals
    QObject::connect(m_ui->torrentList, &TorrentListWidget::torrentsDeselected,
        [this](QList<TorrentHandle*> const& torrents)
        {
            for (auto th : torrents)
            {
                m_selectedTorrents.removeAll(th);
            }
        });

    QObject::connect(m_ui->torrentList, &TorrentListWidget::torrentsSelected,
        [this](QList<TorrentHandle*> const& torrents)
        {
            m_selectedTorrents.append(torrents);
            m_ui->torrentDetails->update(m_selectedTorrents);
        });

    QObject::connect(m_ui->torrentList,    &QTreeView::customContextMenuRequested,
                     this,                 &MainWindow::onTorrentContextMenu);

    // GeoIP signals
    QObject::connect(m_geo,                &GeoIP::updateRequired,
                     m_geo,                &GeoIP::update);

    // System tray
    QObject::connect(m_trayIcon,           &SystemTrayIcon::addTorrentRequested,
                     this,                 &MainWindow::onFileAddTorrent);

    QObject::connect(m_trayIcon,           &SystemTrayIcon::addMagnetLinkRequested,
                     this,                 &MainWindow::onFileAddMagnetLinks);

    QObject::connect(m_trayIcon,           &SystemTrayIcon::exitRequested,
                     this,                 &MainWindow::close);

    QObject::connect(m_trayIcon,           &SystemTrayIcon::viewPreferencesRequested,
                     this,                 &MainWindow::onViewPreferences);

    QObject::connect(m_trayIcon,           &QSystemTrayIcon::activated,
                     this,                 &MainWindow::onTrayIconActivated);

    // JS engine signals
    QObject::connect(m_jsEngine,           &JsEngine::torrentFilterAdded,
                     this,                 &MainWindow::addTorrentFilter);

    this->setAcceptDrops(true);
    this->setMinimumWidth(250);
    this->setWindowIcon(QIcon(":res/app.ico"));
    this->setWindowTitle("PicoTorrent");

    this->resize(
        m_cfg->getInt("ui.widgets.main_window.width"),
        m_cfg->getInt("ui.widgets.main_window.height"));

    if (m_cfg->getString("ui.widgets.splitter.sizes") != "")
    {
        auto sizesPlain = QString::fromStdString(m_cfg->getString("ui.widgets.splitter.sizes"));
        auto sizes      = sizesPlain.split(",");

        QList<int> splitterSizes;

        for (QString const& part : sizes)
        {
            splitterSizes.push_back(part.toInt());
        }

        m_ui->splitter->setSizes(splitterSizes);
    }

    if (m_cfg->getBool("geoip.enabled"))
    {
        m_geo->load();
    }

    this->showHideDetailsPanel(m_cfg->getBool("ui.show_details_panel"));
    this->showHideStatusBar(m_cfg->getBool("ui.show_status_bar"));

    m_ui->statusBar->updateDhtNodesCount(m_cfg->getBool("enable_dht") ? 0 : -1);
    m_ui->statusBar->updateTorrentCount(m_torrentsCount);

    m_trayIcon->show();

    if (m_cfg->getBool("update_checks.enabled"))
    {
        this->checkForUpdates();
    }

    // Load scripts
    fs::path appScripts = m_env->getApplicationPath() / "scripts";
    fs::path appDataScripts = m_env->getApplicationDataPath() / "scripts";

    m_jsEngine->loadDirectory(appScripts);

    if (appDataScripts != appScripts)
    {
        m_jsEngine->loadDirectory(appDataScripts);
    }
}

MainWindow::~MainWindow()
{
    m_cfg->setInt("ui.widgets.main_window.height", this->height());
    m_cfg->setInt("ui.widgets.main_window.width",  this->width());

    // Store splitter sizes
    QStringList sizes;

    for (int size : m_ui->splitter->sizes())
    {
        sizes << QString::number(size);
    }

    m_cfg->setString("ui.widgets.splitter.sizes", sizes.join(",").toStdString());
}

void MainWindow::addTorrents(std::vector<lt::add_torrent_params>& params)
{
    if (params.empty())
    {
        return;
    }

    std::vector<lt::info_hash_t> hashes;

    // Set up default values for all params

    for (lt::add_torrent_params& p : params)
    {
        p.flags |= lt::torrent_flags::duplicate_is_error;
        p.save_path = m_cfg->getString("default_save_path");

        // If we have a param with an info hash and no torrent info,
        // let the session find metadata for us

        if (
			((p.info_hash.has_v1() && !p.info_hash.v1.is_all_zeros())
				|| (p.info_hash.has_v2() && !p.info_hash.v2.is_all_zeros()))
			&& !p.ti)
        {
            hashes.push_back(p.info_hash);
        }
    }

    if (m_cfg->getBool("skip_add_torrent_dialog"))
    {
        for (lt::add_torrent_params& p : params)
        {
            m_session->addTorrent(p);
        }

        return;
    }

    auto dlg = new AddTorrentDialog(params, m_db, this);
    dlg->open();

    QObject::connect(dlg, &QDialog::accepted,
                     [this, dlg]()
                     {
                         for (lt::add_torrent_params& p : dlg->getParams())
                         {
                             m_session->addTorrent(p);
                         }
                     });

    QObject::connect(m_session, &Session::metadataSearchResult,
                     dlg,       &AddTorrentDialog::refreshMetadata);

    QObject::connect(dlg, &QDialog::finished,
                     dlg, &QDialog::deleteLater);

    m_session->metadataSearch(hashes);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasUrls())
    {
        return;
    }

    QStringList files;

    for (QUrl const& url : event->mimeData()->urls())
    {
        files << url.toLocalFile();
    }

    std::vector<lt::add_torrent_params> params;
    this->parseTorrentFiles(params, files);
    this->addTorrents(params);
}

void MainWindow::handleCommandLine(QStringList const& args)
{
    if (args.count() == 0)
    {
        return;
    }

    QCommandLineParser parser;
    parser.process(args);

    std::vector<lt::add_torrent_params> params;

    for (QString const& arg : parser.positionalArguments())
    {
        if (arg.startsWith("magnet:?xt"))
        {
            this->parseMagnetLinks(params, { arg });
        }
        else
        {
            this->parseTorrentFiles(params, { arg });
        }
    }

    this->addTorrents(params);
}

void MainWindow::parseMagnetLinks(std::vector<lt::add_torrent_params>& params, QStringList const& magnetLinks)
{
    for (QString const& magnetLink : magnetLinks)
    {
        lt::add_torrent_params param;
        lt::error_code ec;

        lt::parse_magnet_uri(magnetLink.toStdString(), param, ec);

        if (ec)
        {
            LOG_F(ERROR, "Failed to parse magnet link: %s", ec.message().c_str());
            continue;
        }

        params.push_back(param);
    }
}

void MainWindow::parseTorrentFiles(std::vector<lt::add_torrent_params>& params, QStringList const& files)
{
    for (QString const& fileName : files)
    {
        fs::path p = fs::absolute(fileName.toStdString());

        lt::error_code ec;
        lt::add_torrent_params param;

        param.ti = std::make_shared<lt::torrent_info>(p.string(), ec);

        if (ec)
        {
            LOG_F(ERROR, "Failed to parse torrent file: %s", ec.message().c_str());
            continue;
        }

        params.push_back(param);
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::WindowStateChange:
    {
        bool showInNotificationArea  = m_cfg->getBool("show_in_notification_area");
        bool minimizeToNotificationArea = m_cfg->getBool("minimize_to_notification_area");

        if (this->isMinimized()
            && showInNotificationArea
            && minimizeToNotificationArea)
        {
            this->hide();
            event->accept();
        }

        break;
    }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::addTorrentFilter(pt::ScriptedTorrentFilter* filter)
{
    auto action = m_ui->menuFilter->addAction(filter->name());
    action->setCheckable(true);
    action->setData(QVariant::fromValue(static_cast<void*>(filter)));

    m_filtersGroup->addAction(action);
}

void MainWindow::checkDiskSpace(pt::TorrentHandle* torrent)
{
    bool shouldCheck = m_cfg->getBool("pause_on_low_disk_space");
    int limit = m_cfg->getInt("pause_on_low_disk_space_limit");

    if (!shouldCheck)
    {
        return;
    }

    auto status = torrent->status();

    if (status.paused)
    {
        return;
    }

    ULARGE_INTEGER freeBytesAvailableToCaller;
    ULARGE_INTEGER totalNumberOfBytes;
    ULARGE_INTEGER totalNumberOfFreeBytes;

    BOOL res = GetDiskFreeSpaceEx(
        status.savePath.toStdWString().data(),
        &freeBytesAvailableToCaller,
        &totalNumberOfBytes,
        &totalNumberOfFreeBytes);

    if (res)
    {
        float diskSpaceAvailable = static_cast<float>(freeBytesAvailableToCaller.QuadPart) / static_cast<float>(totalNumberOfBytes.QuadPart);
        float diskSpaceLimit = limit / 100.0f;

        if (diskSpaceAvailable < diskSpaceLimit)
        {
            LOG_F(INFO, "Pausing torrent %s due to disk space too low (avail: %.2f, limit: %.2f)",
                status.infoHash.toStdString().data(),
                diskSpaceAvailable,
                diskSpaceLimit);

            torrent->pause();

            m_trayIcon->showMessage(
                i18n("pause_on_low_disk_space_alert"),
                status.name,
                QIcon(":res/app.ico"));
        }
    }
}

void MainWindow::checkForUpdates(bool force)
{
    auto checker = new UpdateChecker(m_cfg, force);
    checker->check();

    QObject::connect(checker, &UpdateChecker::finished,
                     this,    &MainWindow::showUpdateDialog);

    QObject::connect(checker, &UpdateChecker::finished,
                     checker, &UpdateChecker::deleteLater);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    bool showInNotificationArea  = m_cfg->getBool("show_in_notification_area");
    bool closeToNotificationArea = m_cfg->getBool("close_to_notification_area");

    if (event->spontaneous()
        && showInNotificationArea
        && closeToNotificationArea)
    {
        this->hide();
        event->ignore();
    }
}

bool MainWindow::nativeEvent(QByteArray const& eventType, void* message, long* result)
{
    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_COPYDATA)
    {
        COPYDATASTRUCT* cds = reinterpret_cast<COPYDATASTRUCT*>(msg->lParam);
        const char* encodedArgs = reinterpret_cast<const char*>(cds->lpData);

        picojson::value encodedValue;
        std::string err = picojson::parse(encodedValue, encodedArgs);

        if (!err.empty())
        {
            return false;
        }

        picojson::array arr = encodedValue.get<picojson::array>();
        QStringList args;

        for (picojson::value const& val : arr)
        {
            args.push_back(QString::fromStdString(val.get<std::string>()));
        }

        handleCommandLine(args);

        return true;
    }

    return false;
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);

    if (!m_taskbarButton->window())
    {
        m_taskbarButton->setWindow(this->windowHandle());
    }
}

/* Actions */
void MainWindow::onFileAddMagnetLinks()
{
    auto dlg = new TextInputDialog(this, i18n("magnet_link_s"), true);
    dlg->setWindowTitle(i18n("add_magnet_link_s"));
    dlg->open();

    QClipboard* clipboard = QApplication::clipboard();
    QString text = clipboard->text();
    if (text.startsWith("magnet:", Qt::CaseSensitivity::CaseInsensitive))
    {
        dlg->setText(text);
    }

    QObject::connect(dlg, &TextInputDialog::accepted,
                     [this, dlg]()
                     {
                         auto text  = dlg->text();
                         auto links = text.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

                         std::vector<lt::add_torrent_params> params;
                         this->parseMagnetLinks(params, links);
                         this->addTorrents(params);
                     });

    QObject::connect(dlg, &TextInputDialog::finished,
                     dlg, &TextInputDialog::deleteLater);
}

void MainWindow::onFileAddTorrent()
{
    QStringList filters;
    filters << "Torrent files (*.torrent)"
            << "All files (*)";

    auto dlg = new QFileDialog(this);
    dlg->setFileMode(QFileDialog::ExistingFiles);
    dlg->setNameFilters(filters);
    dlg->open();

    QObject::connect(dlg, &QFileDialog::accepted,
                     [this, dlg]()
                     {
                         std::vector<lt::add_torrent_params> params;
                         this->parseTorrentFiles(params, dlg->selectedFiles());
                         this->addTorrents(params);
                     });

    QObject::connect(dlg, &QFileDialog::finished,
                     dlg, &QFileDialog::deleteLater);
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
    QModelIndex idx = m_ui->torrentList->indexAt(point);

    if (idx.isValid())
    {
        auto menu = new TorrentContextMenu(this, m_selectedTorrents);
        menu->popup(m_ui->torrentList->viewport()->mapToGlobal(point));

        QObject::connect(menu, &QMenu::aboutToHide,
                         menu, &QMenu::deleteLater);
    }
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->showNormal();
        this->activateWindow();
        break;
    }
}

void MainWindow::onViewPreferences()
{
    auto dlg = new PreferencesDialog(this, m_cfg, m_env);
    dlg->load();
    dlg->open();

    QObject::connect(dlg,       &QDialog::accepted,
                     m_session, &Session::reloadSettings);

    QObject::connect(dlg,       &QDialog::finished,
                     dlg,       &QDialog::deleteLater);
}

void MainWindow::updateTaskbarButton(pt::TorrentStatistics* stats)
{
    auto progress = m_taskbarButton->progress();

    if (stats->totalWanted > 0)
    {
        if (!progress->isVisible())
        {
            progress->setVisible(true);
        }

        // Clamp progress to an integer between 0 and 100.
        float totalProgress = 0;

        if (stats->totalWantedDone > 0 && stats->totalWanted > 0)
        {
            totalProgress = (float)stats->totalWantedDone / (float)stats->totalWanted;
        }

        progress->setMaximum(100);
        progress->setValue((int)(totalProgress * 100));
    }
    else
    {
        if (progress->isVisible())
        {
            progress->setVisible(false);
        }
    }
}

void MainWindow::showHideDetailsPanel(bool show)
{
    if (show)
    {
        m_ui->torrentDetails->show();
        this->setMinimumHeight(300);
    }
    else
    {
        m_ui->torrentDetails->hide();
        this->setMinimumHeight(150);
    }

    m_cfg->setBool("ui.show_details_panel", show);
}

void MainWindow::showHideStatusBar(bool show)
{
    if (show)
    {
        m_ui->statusBar->show();
    }
    else
    {
        m_ui->statusBar->hide();
    }

    m_cfg->setBool("ui.show_status_bar", show);
}

void MainWindow::setTorrentFilter(QAction* action)
{
    auto data   = action->data();

    if (data.isNull())
    {
        m_torrentSortFilterModel->setScriptedFilter(nullptr);
        return;
    }

    auto filter = static_cast<ScriptedTorrentFilter*>(data.value<void*>());

    m_torrentSortFilterModel->setScriptedFilter(filter);
}

void MainWindow::showTorrentFinishedNotification(pt::TorrentHandle* torrent)
{
    TorrentStatus status = torrent->status();

    m_trayIcon->showMessage(
        i18n("torrent_finished"),
        status.name,
        QIcon(":res/app.ico"));
}

void MainWindow::showUpdateDialog(pt::UpdateInformation* info)
{
    if (info->available)
    {
        // Load translations
        std::wstring content = i18n("new_version_available").toStdWString();
        std::wstring mainFormatted = i18n("picotorrent_v_available").arg(info->version).toStdWString();
        std::wstring verification = i18n("ignore_update").toStdWString();
        std::wstring show = i18n("show_on_github").toStdWString();

        const TASKDIALOG_BUTTON pButtons[] =
        {
            { 1000, show.c_str() },
        };

        TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
        tdf.cButtons = ARRAYSIZE(pButtons);
        tdf.dwCommonButtons = TDCBF_CLOSE_BUTTON;
        tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW | TDF_USE_COMMAND_LINKS;
        tdf.hwndParent = (HWND)this->winId();
        tdf.pButtons = pButtons;
        tdf.pszMainIcon = TD_INFORMATION_ICON;
        tdf.pszMainInstruction = mainFormatted.c_str();
        tdf.pszVerificationText = verification.c_str();
        tdf.pszWindowTitle = TEXT("PicoTorrent");

        int pnButton = -1;
        int pnRadioButton = -1;
        BOOL pfVerificationFlagChecked = FALSE;

        TaskDialogIndirect(&tdf, &pnButton, &pnRadioButton, &pfVerificationFlagChecked);

        if (pnButton == 1000)
        {
            QDesktopServices::openUrl(QUrl(info->url));
        }

        if (pfVerificationFlagChecked)
        {
            m_cfg->setString(
                "update_checks.ignored_version",
                info->version.toStdString());
        }
    }
    else
    {
        std::wstring main = i18n("no_update_available").toStdWString();

        TASKDIALOGCONFIG tdf = { sizeof(TASKDIALOGCONFIG) };
        tdf.dwCommonButtons = TDCBF_OK_BUTTON;
        tdf.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW;
        tdf.hwndParent = (HWND)this->winId();
        tdf.pszMainIcon = TD_INFORMATION_ICON;
        tdf.pszMainInstruction = main.c_str();
        tdf.pszWindowTitle = TEXT("PicoTorrent");

        TaskDialogIndirect(&tdf, nullptr, nullptr, nullptr);
    }
}
