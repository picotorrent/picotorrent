#include "mainframe.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/write_resume_data.hpp>

#include <filesystem>
#include <fstream>
#include <limits>

#include <wx/aboutdlg.h>
#include <wx/dataview.h>
#include <wx/filedlg.h>
#include <wx/notebook.h>
#include <wx/persist.h>
#include <wx/persist/toplevel.h>
#include <wx/splitter.h>
#include <wx/taskbarbutton.h>

#include "applicationoptions.hpp"
#include "addtorrentproc.hpp"
#include "config.hpp"
#include "environment.hpp"
#include "mainmenu.hpp"
#include "sessionloader.hpp"
#include "sessionstate.hpp"
#include "sessionunloader.hpp"
#include "statusbar.hpp"
#include "taskbaricon.hpp"
#include "torrentcontextmenu.hpp"
#include "torrentdetailsview.hpp"
#include "torrentlistview.hpp"
#include "torrentlistviewmodel.hpp"

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using pt::MainFrame;

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_DATAVIEW_COLUMN_SORTED(ptID_TORRENT_LIST_VIEW, MainFrame::OnTorrentSorted)
    EVT_DATAVIEW_ITEM_CONTEXT_MENU(ptID_TORRENT_LIST_VIEW, MainFrame::OnTorrentContextMenu)
    EVT_DATAVIEW_SELECTION_CHANGED(ptID_TORRENT_LIST_VIEW, MainFrame::OnTorrentSelectionChanged)
    EVT_DROP_FILES(MainFrame::OnDropFiles)
    EVT_ICONIZE(MainFrame::OnIconize)
    EVT_TIMER(ptID_MAIN_TIMER, MainFrame::OnTimer)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(std::shared_ptr<pt::Configuration> config,
    std::shared_ptr<pt::Environment> env,
    std::shared_ptr<pt::Translator> translator)
    : wxFrame(NULL, wxID_ANY, "PicoTorrent"),
    m_config(config),
    m_env(env),
    m_splitter(new wxSplitterWindow(this, wxID_ANY)),
    m_status(new StatusBar(this)),
    m_torrentListViewModel(new TorrentListViewModel(translator)),
    m_trans(translator)
{
    m_state = SessionLoader::Load(m_env, m_config);
    m_state->session->set_alert_notify([this]()
    {
        this->GetEventHandler()->CallAfter(std::bind(&MainFrame::OnSessionAlert, this));
    });

    // Create UI
    m_torrentListView = new TorrentListView(m_splitter, ptID_TORRENT_LIST_VIEW, m_trans);
    m_torrentListView->AssociateModel(m_torrentListViewModel);
    m_torrentDetailsView = new TorrentDetailsView(m_splitter, m_trans, m_state);

    // Splitter
    m_splitter->SetMinimumPaneSize(10);
    m_splitter->SetSashGravity(0.5);
    m_splitter->SplitHorizontally(m_torrentListView, m_torrentDetailsView);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_splitter, 1, wxEXPAND, 0);
    mainSizer->SetSizeHints(this);

    // Task bar icon
    m_taskBar = std::make_shared<TaskBarIcon>(this, m_config, m_env, m_trans, m_state);

    if (m_config->UI()->ShowInNotificationArea())
    {
        m_taskBar->SetPicoIcon();
    }

    this->DragAcceptFiles(true);
    this->SetIcon(wxICON(AppIcon));
    this->SetMenuBar(new MainMenu(m_state, m_config, m_env, m_taskBar, m_trans));
    this->SetName("MainFrame");
    this->SetSizerAndFit(mainSizer);
    this->SetStatusBar(m_status);

    wxPersistenceManager::Get().RegisterAndRestore(this);
    wxPersistenceManager::Get().RegisterAndRestore(m_torrentListView);

    m_timer = new wxTimer(this, ptID_MAIN_TIMER);
    m_timer->Start(1000);
}

MainFrame::~MainFrame()
{
    m_timer->Stop();

    m_state->session->set_alert_notify([] {});
    SessionUnloader::Unload(m_state, m_env);
}

void MainFrame::HandleOptions(std::shared_ptr<pt::ApplicationOptions> options)
{
    AddTorrentProcedure addProc(this, m_config, m_trans, m_state);

    if (!options->files.IsEmpty() && options->magnet_links.IsEmpty())
    {
        addProc.Execute(options->files);
    }
    else if (!options->magnet_links.IsEmpty() && options->files.IsEmpty())
    {
        addProc.ExecuteMagnet(options->magnet_links);
    }
}

void MainFrame::OnClose(wxCloseEvent& ev)
{
    if (ev.CanVeto()
        && m_config->UI()->ShowInNotificationArea()
        && m_config->UI()->CloseToNotificationArea())
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

        ev.Skip();
    }
}

void MainFrame::OnDropFiles(wxDropFilesEvent& ev)
{
    if (ev.GetNumberOfFiles() <= 0)
    {
        return;
    }

    wxString* dropped = ev.GetFiles();
    wxArrayString files;

    for (int i = 0; i < ev.GetNumberOfFiles(); i++)
    {
        files.Add(dropped[i]);
    }

    AddTorrentProcedure proc(this, m_config, m_trans, m_state);
    proc.Execute(files);
}

void MainFrame::OnIconize(wxIconizeEvent& ev)
{
    if (ev.IsIconized()
        && m_config->UI()->ShowInNotificationArea()
        && m_config->UI()->MinimizeToNotificationArea())
    {
        MSWGetTaskBarButton()->Hide();
    }
}

void MainFrame::OnSessionAlert()
{
    std::vector<lt::alert*> alerts;
    m_state->session->pop_alerts(&alerts);

    for (lt::alert* alert : alerts)
    {
        wxLogDebug("%s", alert->message().c_str());

        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (ata->error)
            {
                wxLogError("Error when adding torrent: %s", ata->error.message().c_str());
                break;
            }

            if (ata->handle.torrent_file())
            {
                fs::path torrentsDirectory = m_env->GetApplicationDataPath() / "Torrents";
                if (!fs::exists(torrentsDirectory)) { fs::create_directories(torrentsDirectory); }

                lt::create_torrent ct(*ata->handle.torrent_file());
                lt::entry entry = ct.generate();

                std::stringstream hex;
                hex << ata->handle.info_hash();

                fs::path torrentFile = torrentsDirectory / (hex.str() + ".torrent");
                std::ofstream out(torrentFile, std::ios::binary | std::ios::out);
                lt::bencode(std::ostreambuf_iterator<char>(out), entry);

                // Generate a save resume data alert to save torrent state
                ata->handle.save_resume_data();
            }

            m_state->torrents.insert({ ata->handle.info_hash(), ata->handle });
            m_status->UpdateTorrentCount(m_state->torrents.size());
            m_torrentListViewModel->Add(ata->handle.status());
            m_torrentListView->Sort();

            break;
        }
        case lt::metadata_received_alert::alert_type:
        {
            lt::metadata_received_alert* mra = lt::alert_cast<lt::metadata_received_alert>(alert);

            fs::path torrentsDirectory = m_env->GetApplicationDataPath() / "Torrents";
            if (!fs::exists(torrentsDirectory)) { fs::create_directories(torrentsDirectory); }

            lt::create_torrent ct(*mra->handle.torrent_file());
            lt::entry entry = ct.generate();

            std::stringstream hex;
            hex << mra->handle.info_hash();

            fs::path torrentFile = torrentsDirectory / (hex.str() + ".torrent");
            std::ofstream out(torrentFile, std::ios::binary | std::ios::out);
            lt::bencode(std::ostreambuf_iterator<char>(out), entry);

            // Generate a save resume data alert to save torrent state
            mra->handle.save_resume_data();

            m_torrentListViewModel->Update(mra->handle.status());
            m_torrentListView->Sort();

            if (m_state->IsSelected(mra->handle.info_hash()))
            {
                m_torrentDetailsView->Clear();
                m_torrentDetailsView->Update();
            }

            break;
        }
        case lt::save_resume_data_alert::alert_type:
        {
            lt::save_resume_data_alert* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);

            fs::path torrentsDirectory = m_env->GetApplicationDataPath() / "Torrents";
            if (!fs::exists(torrentsDirectory)) { fs::create_directories(torrentsDirectory); }

            std::stringstream hex;
            hex << srda->handle.info_hash();

            lt::entry entry = lt::write_resume_data(srda->params);
            std::vector<char> buf;
            lt::bencode(std::back_inserter(buf), entry);

            fs::path datFile = torrentsDirectory / (hex.str() + ".dat");
            std::ofstream out(datFile, std::ios::binary | std::ios::out);
            std::copy(
                buf.begin(),
                buf.end(),
                std::ostreambuf_iterator<char>(out));

            break;
        }
        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
            lt::span<const int64_t> counters = ssa->counters();
            int idx = -1;

            if ((idx = lt::find_metric_idx("dht.dht_nodes")) >= 0)
            {
                m_status->UpdateDhtNodesCount(counters[idx]);
            }

            break;
        }
        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);

            int dl_count = 0;
            float dl_progress = 0;
            int64_t dl_rate = 0;
            int64_t ul_rate = 0;

            for (lt::torrent_status const& ts : sua->status)
            {
                m_torrentListViewModel->Update(ts);

                dl_rate += ts.download_payload_rate;
                ul_rate += ts.upload_payload_rate;

                if (ts.state & lt::torrent_status::state_t::downloading)
                {
                    dl_count += 1;
                    dl_progress = ts.progress;
                }
            }

            if (sua->status.size() > 0)
            {
                m_torrentListView->Sort();
            }

            m_status->UpdateTorrentCount(m_state->torrents.size());
            m_status->UpdateTransferRates(dl_rate, ul_rate);

            wxTaskBarButton* tbb = MSWGetTaskBarButton();

            if (dl_progress > 0)
            {
                tbb->SetProgressState(wxTaskBarButtonState::wxTASKBAR_BUTTON_NORMAL);
                tbb->SetProgressRange(dl_count * 1000);
                tbb->SetProgressValue(static_cast<int>(dl_progress * 1000));
            }
            else
            {
                tbb->SetProgressState(wxTaskBarButtonState::wxTASKBAR_BUTTON_NO_PROGRESS);
            }

            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);

            m_state->torrents.erase(tra->info_hash);
            m_status->UpdateTorrentCount(m_state->torrents.size());
            m_torrentListViewModel->Remove(tra->info_hash);
            m_torrentDetailsView->Clear();

            // Remove the torrent and dat file from disk
            std::stringstream hex;
            hex << tra->info_hash;

            std::string hash = hex.str();
            fs::path torrents_dir = fs::path(m_env->GetApplicationDataPath() / "Torrents");
            fs::path torrent_file = torrents_dir / fs::path(hash + ".torrent");
            fs::path torrent_dat = torrents_dir / fs::path(hash + ".dat");

            if (fs::exists(torrent_file)) { fs::remove(torrent_file); }
            if (fs::exists(torrent_dat)) { fs::remove(torrent_dat); }

            m_torrentListView->Sort();

            break;
        }
        }
    }
}

void MainFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    m_state->session->post_dht_stats();
    m_state->session->post_session_stats();
    m_state->session->post_torrent_updates();

    m_torrentDetailsView->Update();
}

void MainFrame::OnTorrentContextMenu(wxDataViewEvent& event)
{
    if (m_state->selected_torrents.empty())
    {
        return;
    }

    TorrentContextMenu menu(this, m_trans, m_state);
    PopupMenu(&menu);
}

void MainFrame::OnTorrentSelectionChanged(wxDataViewEvent& event)
{
    wxDataViewItemArray items;
    m_torrentListView->GetSelections(items);

    m_state->selected_torrents.clear();
    m_torrentDetailsView->Clear();
    
    if (items.IsEmpty())
    {
        return;
    }

    for (wxDataViewItem& item : items)
    {
        unsigned int row = m_torrentListViewModel->GetRow(item);

        lt::sha1_hash hash = m_torrentListViewModel->FindHashByRow(row);
        lt::torrent_handle th = m_state->torrents.at(hash);

        m_state->selected_torrents.push_back(th);
    }

    m_torrentDetailsView->Update();
}

void MainFrame::OnTorrentSorted(wxDataViewEvent& event)
{
    m_torrentListView->Sort();
}
