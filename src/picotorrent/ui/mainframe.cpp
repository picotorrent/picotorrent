#include "mainframe.hpp"

#include <filesystem>

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <loguru.hpp>
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>

#include "../bittorrent/session.hpp"
#include "../bittorrent/sessionstatistics.hpp"
#include "../bittorrent/torrenthandle.hpp"
#include "../core/configuration.hpp"
#include "../core/database.hpp"
#include "../core/environment.hpp"
#include "dialogs/aboutdialog.hpp"
#include "dialogs/addmagnetlinkdialog.hpp"
#include "dialogs/addtorrentdialog.hpp"
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
    m_torrentList(new TorrentListView(m_splitter, ptID_MAIN_TORRENT_LIST)),
    m_torrentListModel(new Models::TorrentListModel()),
    m_torrentsCount(0),
    m_updateChecker(new UpdateChecker(this, cfg))
{
    m_splitter->SetMinimumPaneSize(10);
    m_splitter->SetSashGravity(0.5);
    m_splitter->SplitHorizontally(
        m_torrentList,
        m_torrentDetails);

    m_torrentList->AssociateModel(m_torrentListModel);
    m_torrentListModel->DecRef();

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

    this->Bind(ptEVT_TORRENT_UPDATED, [this](wxCommandEvent& evt)
        {
            auto torrent = static_cast<BitTorrent::TorrentHandle*>(evt.GetClientData());
            m_torrentListModel->UpdateTorrent(torrent);

            // If this torrent is in our selection, update the details view
            auto iter = m_selection.find(torrent->InfoHash());

            if (iter != m_selection.end())
            {
                m_torrentDetails->Refresh({ *iter });
            }
        });

    this->Bind(wxEVT_DATAVIEW_COLUMN_SORTED, [this](wxDataViewEvent&) { m_torrentList->Sort(); }, ptID_MAIN_TORRENT_LIST);

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
    this->Bind(wxEVT_MENU, &MainFrame::OnFileAddTorrent, this, ptID_EVT_ADD_TORRENT);
    this->Bind(wxEVT_MENU, &MainFrame::OnFileAddMagnetLink, this, ptID_EVT_ADD_MAGNET_LINK);
    this->Bind(wxEVT_MENU, [this](wxCommandEvent&) { this->Close(true); }, ptID_EVT_EXIT);
    this->Bind(wxEVT_MENU, &MainFrame::OnHelpAbout, this, ptID_EVT_ABOUT);

    m_taskBarIcon->Bind(wxEVT_MENU, &MainFrame::OnFileAddTorrent, this, ptID_EVT_ADD_TORRENT);
    m_taskBarIcon->Bind(wxEVT_MENU, &MainFrame::OnFileAddMagnetLink, this, ptID_EVT_ADD_MAGNET_LINK);
    m_taskBarIcon->Bind(wxEVT_MENU, [this](wxCommandEvent&) { this->Close(true); }, ptID_EVT_EXIT);

    this->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent&)
        {
            m_updateChecker->Check(true);
        },
        ptID_EVT_CHECK_FOR_UPDATE);

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

    // Check for updates
    if (m_cfg->GetBool("update_checks.enabled"))
    {
        m_updateChecker->Check();
    }
}

MainFrame::~MainFrame()
{
    m_taskBarIcon->Hide();
    this->Hide();

    delete m_taskBarIcon;
    delete m_session;
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

wxMenuBar* MainFrame::CreateMainMenu()
{
    auto fileMenu = new wxMenu();
    fileMenu->Append(ptID_EVT_ADD_TORRENT, i18n("amp_add_torrent"));
    fileMenu->Append(ptID_EVT_ADD_MAGNET_LINK, i18n("amp_add_magnet_link_s"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ptID_EVT_EXIT, i18n("amp_exit"));

    auto viewMenu = new wxMenu();
    // TODO filter
    m_menuItemDetailsPanel = viewMenu->Append(ptID_EVT_SHOW_DETAILS, i18n("amp_details_panel"));
    m_menuItemStatusBar = viewMenu->Append(ptID_EVT_SHOW_STATUS_BAR, i18n("amp_status_bar"));
    viewMenu->AppendSeparator();
    viewMenu->Append(ptID_EVT_VIEW_PREFERENCES, i18n("amp_preferences"));

    auto helpMenu = new wxMenu();
    helpMenu->Append(ptID_EVT_CHECK_FOR_UPDATE, i18n("amp_check_for_update"));
    helpMenu->AppendSeparator();
    helpMenu->Append(ptID_EVT_ABOUT, i18n("amp_about"));

    auto mainMenu = new wxMenuBar();
    mainMenu->Append(fileMenu, i18n("amp_file"));
    mainMenu->Append(viewMenu, i18n("amp_view"));
    mainMenu->Append(helpMenu, i18n("amp_help"));

    return mainMenu;
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
        "add_torrent_s",
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
