#include "mainframe.hpp"

#include <filesystem>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <loguru.hpp>
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/taskbarbutton.h>

#include "../bittorrent/session.hpp"
#include "../bittorrent/sessionstatistics.hpp"
#include "../bittorrent/torrenthandle.hpp"
#include "../bittorrent/torrentstatistics.hpp"
#include "../bittorrent/torrentstatus.hpp"
#include "../core/configuration.hpp"
#include "../core/database.hpp"
#include "../core/environment.hpp"
#include "../core/utils.hpp"
#include "dialogs/aboutdialog.hpp"
#include "dialogs/addmagnetlinkdialog.hpp"
#include "dialogs/addtorrentdialog.hpp"
#include "dialogs/preferencesdialog.hpp"
#include "ids.hpp"
#include "models/torrentlistmodel.hpp"
#include "statusbar.hpp"
#include "taskbaricon.hpp"
#include "torrentcontextmenu.hpp"
#include "torrentdetailsview.hpp"
#include "torrentlistview.hpp"
#include "translator.hpp"
#include "../updatechecker.hpp"

namespace fs = std::filesystem;
using pt::UI::MainFrame;

const char* WindowTitle = "PicoTorrent";

MainFrame::MainFrame(std::shared_ptr<Core::Environment> env, std::shared_ptr<Core::Database> db, std::shared_ptr<Core::Configuration> cfg)
    : wxFrame(nullptr, wxID_ANY, WindowTitle, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, "MainFrame"),
    m_env(env),
    m_db(db),
    m_cfg(cfg),
    m_session(new BitTorrent::Session(this, db, cfg, env)),
    m_splitter(new wxSplitterWindow(this, ptID_MAIN_SPLITTER)),
    m_statusBar(new StatusBar(this)),
    m_taskBarIcon(new TaskBarIcon(this)),
    m_torrentDetails(new TorrentDetailsView(m_splitter, ptID_MAIN_TORRENT_DETAILS)),
    m_torrentListModel(new Models::TorrentListModel()),
    m_torrentList(new TorrentListView(m_splitter, ptID_MAIN_TORRENT_LIST, m_torrentListModel)),
    m_torrentsCount(0),
    m_menuItemFilters(nullptr)
{
    m_splitter->SetMinimumPaneSize(10);
    m_splitter->SetSashGravity(0.5);
    m_splitter->SplitHorizontally(
        m_torrentList,
        m_torrentDetails);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_splitter, 1, wxEXPAND, 0);
    sizer->SetSizeHints(this);

    this->SetIcon(wxICON(AppIcon));
    this->SetMenuBar(this->CreateMainMenu());
    this->SetSizerAndFit(sizer);
    this->SetStatusBar(m_statusBar);

    // Set checked on menu items
    m_menuItemDetailsPanel->SetCheckable(true);
    m_menuItemDetailsPanel->Check(m_cfg->GetBool("ui.show_details_panel"));
    m_menuItemStatusBar->SetCheckable(true);
    m_menuItemStatusBar->Check(m_cfg->GetBool("ui.show_status_bar"));

    if (!m_cfg->GetBool("ui.show_details_panel")) { m_splitter->Unsplit(); }
    if (!m_cfg->GetBool("ui.show_status_bar")) { m_statusBar->Hide(); }

    if (!wxPersistenceManager::Get().RegisterAndRestore(this))
    {
        this->SetSize(FromDIP(wxSize(450, 400)));
    }

    // Session events
    this->Bind(ptEVT_SESSION_STATISTICS, [this](pt::BitTorrent::SessionStatisticsEvent& evt)
        {
            bool dhtEnabled = m_cfg->GetBool("enable_dht");
            m_statusBar->UpdateDhtNodesCount(dhtEnabled ? evt.GetData().dhtNodes : -1);
        });

    this->Bind(ptEVT_TORRENT_ADDED, [this](wxCommandEvent& evt)
        {
            m_torrentsCount++;
            m_statusBar->UpdateTorrentCount(m_torrentsCount);
            m_torrentListModel->AddTorrent(static_cast<BitTorrent::TorrentHandle*>(evt.GetClientData()));
        });

    this->Bind(ptEVT_TORRENT_FINISHED, [this](wxCommandEvent& evt)
        {
            auto torrent = static_cast<BitTorrent::TorrentHandle*>(evt.GetClientData());
            m_taskBarIcon->ShowBalloon(
                i18n("torrent_finished"),
                torrent->Status().name);
        });

    this->Bind(ptEVT_TORRENT_REMOVED, [this](pt::BitTorrent::InfoHashEvent& evt)
        {
            m_torrentsCount--;
            m_statusBar->UpdateTorrentCount(m_torrentsCount);
            m_torrentListModel->RemoveTorrent(evt.GetData());

            // If this torrent is in our selection, remove it and clear the details view
            auto iter = m_selection.find(evt.GetData());

            if (iter != m_selection.end())
            {
                m_selection.erase(iter);
                m_torrentDetails->Reset();
            }
        });

    this->Bind(ptEVT_TORRENT_STATISTICS, [this](pt::BitTorrent::TorrentStatisticsEvent& evt)
        {
            auto stats = evt.GetData();

            m_statusBar->UpdateTransferRates(
                stats.totalPayloadDownloadRate,
                stats.totalPayloadUploadRate);

            if (wxTaskBarButton* tbb = MSWGetTaskBarButton())
            {
                if (stats.isDownloadingAny && stats.totalWanted > 0)
                {
                    float totalProgress = stats.totalWantedDone / static_cast<float>(stats.totalWanted);

                    tbb->SetProgressState(wxTaskBarButtonState::wxTASKBAR_BUTTON_NORMAL);
                    tbb->SetProgressRange(static_cast<int>(100));
                    tbb->SetProgressValue(static_cast<int>(totalProgress * 100));
                }
                else
                {
                    tbb->SetProgressState(wxTaskBarButtonState::wxTASKBAR_BUTTON_NO_PROGRESS);
                }
            }
        });

    this->Bind(ptEVT_TORRENTS_UPDATED, [this](pt::BitTorrent::TorrentsUpdatedEvent& evt)
        {
            auto torrents = evt.GetData();
            m_torrentListModel->UpdateTorrents(torrents);

            std::map<lt::info_hash_t, pt::BitTorrent::TorrentHandle*> selectedUpdated;

            for (auto torrent : torrents)
            {
                auto infoHash = torrent->InfoHash();

                if (m_selection.find(infoHash) != m_selection.end())
                {
                    selectedUpdated.insert({ infoHash, torrent });
                }
            }

            if (selectedUpdated.size() > 0)
            {
                m_torrentDetails->Refresh(selectedUpdated);
            }

            this->CheckDiskSpace(torrents);
        });

    this->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MainFrame::ShowTorrentContextMenu, this, ptID_MAIN_TORRENT_LIST);

    this->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, [this](wxCommandEvent& evt)
        {
            wxDataViewItemArray items;
            m_torrentList->GetSelections(items);

            m_selection.clear();

            if (items.IsEmpty())
            {
                m_torrentDetails->Reset();
                return;
            }

            for (wxDataViewItem& item : items)
            {
                auto torrent = m_torrentListModel->GetTorrentFromItem(item);
                m_selection.insert({ torrent->InfoHash(), torrent });
            }

            m_torrentDetails->Refresh(m_selection);
        }, ptID_MAIN_TORRENT_LIST);

    // connect events
    this->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this, wxID_ANY);
    this->Bind(wxEVT_ICONIZE, &MainFrame::OnIconize, this, wxID_ANY);
    this->Bind(wxEVT_MENU, &MainFrame::OnFileAddTorrent, this, ptID_EVT_ADD_TORRENT);
    this->Bind(wxEVT_MENU, &MainFrame::OnFileAddMagnetLink, this, ptID_EVT_ADD_MAGNET_LINK);
    this->Bind(wxEVT_MENU, [this](wxCommandEvent&) { this->Close(true); }, ptID_EVT_EXIT);
    this->Bind(wxEVT_MENU, &MainFrame::OnViewPreferences, this, ptID_EVT_VIEW_PREFERENCES);
    this->Bind(wxEVT_MENU, &MainFrame::OnHelpAbout, this, ptID_EVT_ABOUT);

    m_taskBarIcon->Bind(wxEVT_MENU, &MainFrame::OnFileAddTorrent, this, ptID_EVT_ADD_TORRENT);
    m_taskBarIcon->Bind(wxEVT_MENU, &MainFrame::OnFileAddMagnetLink, this, ptID_EVT_ADD_MAGNET_LINK);
    m_taskBarIcon->Bind(wxEVT_MENU, [this](wxCommandEvent&) { this->Close(true); }, ptID_EVT_EXIT);
    m_taskBarIcon->Bind(wxEVT_MENU, &MainFrame::OnViewPreferences, this, ptID_EVT_VIEW_PREFERENCES);
    m_taskBarIcon->Bind(wxEVT_TASKBAR_LEFT_DOWN, &MainFrame::OnTaskBarLeftDown, this, wxID_ANY);

    this->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            m_cfg->SetBool("ui.show_details_panel", m_menuItemDetailsPanel->IsChecked());

            if (m_menuItemDetailsPanel->IsChecked())
            {
                m_splitter->SplitHorizontally(
                    m_torrentList,
                    m_torrentDetails);
            }
            else
            {
                m_splitter->Unsplit();
            }
        }, ptID_EVT_SHOW_DETAILS);

    this->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            m_cfg->SetBool("ui.show_status_bar", m_menuItemStatusBar->IsChecked());

            if (m_menuItemStatusBar->IsChecked()) { m_statusBar->Show(); }
            else { m_statusBar->Hide(); }
            this->SendSizeEvent();
        }, ptID_EVT_SHOW_STATUS_BAR);

    // Update status bar
    m_statusBar->UpdateDhtNodesCount(m_cfg->GetBool("enable_dht") ? 0 : -1);
    m_statusBar->UpdateTorrentCount(m_torrentsCount);

    // Show taskbar icon
    if (m_cfg->GetBool("show_in_notification_area"))
    {
        m_taskBarIcon->Show();
    }
}

MainFrame::~MainFrame()
{
    m_taskBarIcon->Hide();

    delete m_taskBarIcon;
    delete m_session;
}

void MainFrame::AddFilter(wxString const& name, std::function<bool(BitTorrent::TorrentHandle*)> const& filter)
{
    if (m_menuItemFilters == nullptr)
    {
        m_filtersMenu = new wxMenu();
        m_filtersMenu->AppendRadioItem(ptID_EVT_FILTERS_NONE, i18n("amp_none"));
        m_filtersMenu->Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& evt)
            {
                auto filter = m_filters.find(evt.GetId());
                if (filter == m_filters.end())
                {
                    m_torrentListModel->ClearFilter();
                }
                else
                {
                    m_torrentListModel->SetFilter(filter->second);
                }
            });

        m_viewMenu->InsertSeparator(0);
        m_menuItemFilters = m_viewMenu->Insert(0, wxID_ANY, i18n("amp_filter"), m_filtersMenu);
    }

    m_filters.insert({ ptID_EVT_FILTERS_USER + m_filtersMenu->GetMenuItemCount(), filter });

    m_filtersMenu->AppendRadioItem(
        ptID_EVT_FILTERS_USER + m_filtersMenu->GetMenuItemCount(),
        name);
}

void MainFrame::AddTorrents(std::vector<lt::add_torrent_params>& params)
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
        p.save_path = m_cfg->GetString("default_save_path");

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

    if (m_cfg->GetBool("skip_add_torrent_dialog"))
    {
        for (lt::add_torrent_params& p : params)
        {
            m_session->AddTorrent(p);
        }

        return;
    }

    Dialogs::AddTorrentDialog dlg(this, wxID_ANY, params, m_db);

    this->Bind(
        ptEVT_TORRENT_METADATA_FOUND,
        [&dlg](pt::BitTorrent::MetadataFoundEvent& evt)
        {
            dlg.MetadataFound(evt.GetData());
        });

    // search for metadata
    m_session->AddMetadataSearch(hashes);

    if (dlg.ShowModal() == wxID_OK)
    {
        for (lt::add_torrent_params const& p : dlg.GetTorrentParams())
        {
            m_session->AddTorrent(p);
        }
    }
}

void MainFrame::CheckDiskSpace(std::vector<pt::BitTorrent::TorrentHandle*> const& torrents)
{
    bool shouldCheck = m_cfg->GetBool("pause_on_low_disk_space");
    int limit = m_cfg->GetInt("pause_on_low_disk_space_limit");

    if (!shouldCheck)
    {
        return;
    }

    for (auto torrent : torrents)
    {
        auto status = torrent->Status();

        ULARGE_INTEGER freeBytesAvailableToCaller;
        ULARGE_INTEGER totalNumberOfBytes;
        ULARGE_INTEGER totalNumberOfFreeBytes;

        BOOL res = GetDiskFreeSpaceEx(
            Utils::toStdWString(status.savePath).c_str(),
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
                    status.infoHash.c_str(),
                    diskSpaceAvailable,
                    diskSpaceLimit);

                torrent->Pause();

                m_taskBarIcon->ShowBalloon(
                    i18n("pause_on_low_disk_space_alert"),
                    status.name);
            }
        }
    }
}

wxMenuBar* MainFrame::CreateMainMenu()
{
    auto fileMenu = new wxMenu();
    fileMenu->Append(ptID_EVT_ADD_TORRENT, i18n("amp_add_torrent"));
    fileMenu->Append(ptID_EVT_ADD_MAGNET_LINK, i18n("amp_add_magnet_link_s"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ptID_EVT_EXIT, i18n("amp_exit"));

    m_viewMenu = new wxMenu();
    m_menuItemDetailsPanel = m_viewMenu->Append(ptID_EVT_SHOW_DETAILS, i18n("amp_details_panel"));
    m_menuItemStatusBar = m_viewMenu->Append(ptID_EVT_SHOW_STATUS_BAR, i18n("amp_status_bar"));
    m_viewMenu->AppendSeparator();
    m_viewMenu->Append(ptID_EVT_VIEW_PREFERENCES, i18n("amp_preferences"));

    auto helpMenu = new wxMenu();
    helpMenu->Append(ptID_EVT_ABOUT, i18n("amp_about"));

    auto mainMenu = new wxMenuBar();
    mainMenu->Append(fileMenu, i18n("amp_file"));
    mainMenu->Append(m_viewMenu, i18n("amp_view"));
    mainMenu->Append(helpMenu, i18n("amp_help"));

    return mainMenu;
}

void MainFrame::OnClose(wxCloseEvent& evt)
{
    if (evt.CanVeto()
        && m_cfg->GetBool("show_in_notification_area")
        && m_cfg->GetBool("close_to_notification_area"))
    {
        Hide();
        MSWGetTaskBarButton()->Hide();
    }
    else
    {
        // We hide early while closing to not occupy the
        // screen more than necessary. Otherwise the window
        // would be visible (and unresponsive) for a few seconds
        // before being destroyed.
        Hide();

        evt.Skip();
    }
}

void MainFrame::OnFileAddMagnetLink(wxCommandEvent&)
{
    Dialogs::AddMagnetLinkDialog dlg(this, wxID_ANY);

    if (dlg.ShowModal() == wxID_OK)
    {
        this->AddTorrents(dlg.GetParams());
    }
}

void MainFrame::OnFileAddTorrent(wxCommandEvent&)
{
    wxFileDialog openDialog(
        this,
        i18n("add_torrent_s"),
        wxEmptyString,
        wxEmptyString,
        "Torrent files (*.torrent)|*.torrent|All files (*.*)|*.*",
        wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

    if (openDialog.ShowModal() != wxID_OK)
    {
        return;
    }

    wxArrayString paths;
    openDialog.GetPaths(paths);

    std::vector<lt::add_torrent_params> params;
    this->ParseTorrentFiles(params, paths);
    this->AddTorrents(params);
}

void MainFrame::OnHelpAbout(wxCommandEvent&)
{
    Dialogs::AboutDialog dlg(this, wxID_ANY);
    dlg.ShowModal();
}

void MainFrame::OnIconize(wxIconizeEvent& ev)
{
    if (ev.IsIconized()
        && m_cfg->GetBool("show_in_notification_area")
        && m_cfg->GetBool("minimize_to_notification_area"))
    {
        MSWGetTaskBarButton()->Hide();
    }
}

void MainFrame::OnTaskBarLeftDown(wxTaskBarIconEvent&)
{
    this->MSWGetTaskBarButton()->Show();
    this->Restore();
    this->Raise();
    this->Show();
}

void MainFrame::OnViewPreferences(wxCommandEvent&)
{
    Dialogs::PreferencesDialog dlg(this, m_cfg);
    
    if (dlg.ShowModal() == wxID_OK)
    {
        // Reload settings
        m_session->ReloadSettings();

        if (m_cfg->GetBool("show_in_notification_area") && !m_taskBarIcon->IsIconInstalled())
        {
            m_taskBarIcon->Show();
        }
        else if (!m_cfg->GetBool("show_in_notification_area") && m_taskBarIcon->IsIconInstalled())
        {
            m_taskBarIcon->Hide();
        }
    }
}

void MainFrame::ParseTorrentFiles(std::vector<lt::add_torrent_params>& params, wxArrayString const& paths)
{
    for (wxString const& path : paths)
    {
        fs::path p = fs::absolute(path.ToStdWstring());

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

void MainFrame::ShowTorrentContextMenu(wxCommandEvent&)
{
    wxDataViewItemArray items;
    m_torrentList->GetSelections(items);

    if (items.IsEmpty())
    {
        return;
    }

    std::vector<BitTorrent::TorrentHandle*> selectedTorrents;

    for (wxDataViewItem& item : items)
    {
        selectedTorrents.push_back(
            m_torrentListModel->GetTorrentFromItem(item));
    }

    TorrentContextMenu menu(this, selectedTorrents);
    PopupMenu(&menu);
}
