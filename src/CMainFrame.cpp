#include "CMainFrame.hpp"

#include <algorithm>
#include <strsafe.h>

#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include "Commands/FindMetadataCommand.hpp"
#include "Commands/MoveTorrentsCommand.hpp"
#include "Commands/PauseTorrentsCommand.hpp"
#include "Commands/QueueTorrentCommand.hpp"
#include "Commands/RemoveTorrentsCommand.hpp"
#include "Commands/ResumeTorrentsCommand.hpp"
#include "Commands/ShowTorrentDetailsCommand.hpp"
#include "Configuration.hpp"
#include "Core/Torrent.hpp"
#include "Controllers/AddTorrentController.hpp"
#include "Controllers/NotifyIconController.hpp"
#include "Controllers/TorrentDetailsController.hpp"
#include "Controllers/ViewPreferencesController.hpp"
#include "Dialogs/AboutDialog.hpp"
#include "Dialogs/AddMagnetLinkDialog.hpp"
#include "Dialogs/OpenFileDialog.hpp"
#include "Environment.hpp"
#include "Log.hpp"
#include "IO/Directory.hpp"
#include "IO/File.hpp"
#include "IO/Path.hpp"
#include "Models/Torrent.hpp"
#include "Scaler.hpp"
#include "StringUtils.hpp"
#include "Translator.hpp"
#include "UI/MainMenu.hpp"
#include "UI/NotifyIcon.hpp"
#include "UI/StatusBar.hpp"
#include "UI/Taskbar.hpp"
#include "UI/TorrentListView.hpp"

namespace lt = libtorrent;

const UINT CMainFrame::TaskbarButtonCreated = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));

CMainFrame::CMainFrame()
    :
    m_metrics(lt::session_stats_metrics())
{
}

CMainFrame::~CMainFrame()
{
}

void CMainFrame::LoadState()
{
}

LRESULT CMainFrame::OnRegisterNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hWndTarget = reinterpret_cast<HWND>(lParam);
    m_listeners.push_back(hWndTarget);

    return FALSE;
}


LRESULT CMainFrame::OnUnregisterNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND hWndTarget = reinterpret_cast<HWND>(lParam);
    
    auto f = std::find(m_listeners.begin(), m_listeners.end(), hWndTarget);
    if (f != m_listeners.end()) { m_listeners.erase(f); }

    return FALSE;
}

LRESULT CMainFrame::OnFindMetadata(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto cmd = reinterpret_cast<Commands::FindMetadataCommand*>(lParam);

    lt::add_torrent_params p;
    lt::error_code ec;
    lt::parse_magnet_uri(cmd->magnetUri, p, ec);

    if (ec)
    {
        LOG(Error) << "Could not parse magnet link: " << ec.message();
        return FALSE;
    }

    if (std::find(m_find_metadata.begin(), m_find_metadata.end(), p.info_hash) != m_find_metadata.end())
    {
        LOG(Warning) << "Torrent already exists in session: " << p.info_hash;
        return FALSE;
    }

    // Forced start
    p.flags &= ~lt::add_torrent_params::flag_paused;
    p.flags &= ~lt::add_torrent_params::flag_auto_managed;
    p.flags |= lt::add_torrent_params::flag_upload_mode;

    TCHAR temp[MAX_PATH];
    GetTempPath(ARRAYSIZE(temp), temp);

    // Set a temporary save path
    std::string ih = lt::to_hex(p.info_hash.to_string());
    p.save_path = TS(IO::Path::Combine(temp, TWS(ih)));

    // Add the info hash to our list of currently requested metadata files.
    m_find_metadata.push_back({ p.info_hash });
    m_session->async_add_torrent(p);

    return FALSE;
}

LRESULT CMainFrame::OnMoveTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Dialogs::OpenFileDialog dlg;
    dlg.SetOptions(dlg.GetOptions() | FOS_PICKFOLDERS | FOS_PATHMUSTEXIST);
    dlg.SetTitle(TRW("select_destination"));
    dlg.Show();

    auto paths = dlg.GetPaths();
    if (paths.size() == 0) { return FALSE; }

    auto* mv = reinterpret_cast<Commands::MoveTorrentsCommand*>(lParam);

    for (auto&t : mv->torrents)
    {
        const lt::torrent_handle& th = m_torrents.at(t.infoHash);
        th.move_storage(ToString(paths[0]));
    }

    return FALSE;
}

LRESULT CMainFrame::OnPauseTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto ptc = reinterpret_cast<Commands::PauseTorrentsCommand*>(lParam);

    for (auto& t : ptc->torrents)
    {
        const lt::torrent_handle& th = m_torrents.at(t.infoHash);

        th.auto_managed(false);
        th.pause();
    }

    return FALSE;
}

LRESULT CMainFrame::OnQueueTorrent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto qtc = reinterpret_cast<Commands::QueueTorrentCommand*>(lParam);
    const lt::torrent_handle& th = m_torrents.at(qtc->torrent.infoHash);

    switch (qtc->direction)
    {
    case Commands::QueueTorrentCommand::Bottom:
        th.queue_position_bottom();
        break;
    case Commands::QueueTorrentCommand::Down:
        th.queue_position_down();
        break;
    case Commands::QueueTorrentCommand::Top:
        th.queue_position_top();
        break;
    case Commands::QueueTorrentCommand::Up:
        th.queue_position_up();
        break;
    }

    return FALSE;
}

LRESULT CMainFrame::OnRemoveTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto rm = reinterpret_cast<Commands::RemoveTorrentsCommand*>(lParam);



    return FALSE;
}

LRESULT CMainFrame::OnResumeTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto res = reinterpret_cast<Commands::ResumeTorrentsCommand*>(lParam);

    for (auto& t : res->torrents)
    {
        const lt::torrent_handle& th = m_torrents.at(t.infoHash);
        const lt::torrent_status& ts = th.status();

        if (ts.paused && ts.errc)
        {
            th.clear_error();
        }

        th.set_upload_mode(false);
        th.auto_managed(!res->force);
        th.pause();
    }

    return FALSE;
}

LRESULT CMainFrame::OnShowTorrentDetails(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto show = reinterpret_cast<Commands::ShowTorrentDetailsCommand*>(lParam);
    const lt::torrent_handle& th = m_torrents.at(show->torrent.infoHash);

    Controllers::TorrentDetailsController controller(m_hWnd, th);
    controller.Execute();

    return FALSE;
}

LRESULT CMainFrame::OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Controllers::NotifyIconController nic(m_hWnd, m_session);
    nic.Execute(uMsg, wParam, lParam);

    return FALSE;
}

LRESULT CMainFrame::OnTaskbarButtonCreated(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_taskbar = std::make_shared<UI::Taskbar>(m_hWnd);
    return FALSE;
}

void CMainFrame::LoadTorrents()
{
    
}

void CMainFrame::SaveState()
{
    
}

void CMainFrame::SaveTorrents()
{
    m_session->pause();

    // Save each torrents resume data
    int numOutstandingResumeData = 0;
    int numPaused = 0;
    int numFailed = 0;

    std::vector<lt::torrent_status> temp;
    m_session->get_torrent_status(&temp, [](const lt::torrent_status &st) { return true; }, 0);

    for (lt::torrent_status &st : temp)
    {
        if (!st.handle.is_valid()
            || !st.has_metadata
            || !st.need_save_resume)
        {
            continue;
        }

        st.handle.save_resume_data();
        ++numOutstandingResumeData;
    }

    LOG(Info) << "Saving resume data for " << numOutstandingResumeData << " torrent(s)";
    std::wstring torrents_dir = IO::Path::Combine(Environment::GetDataPath(), TEXT("Torrents"));

    if (!IO::Directory::Exists(torrents_dir))
    {
        IO::Directory::Create(torrents_dir);
    }

    while (numOutstandingResumeData > 0)
    {
        const lt::alert *a = m_session->wait_for_alert(lt::seconds(10));
        if (a == 0) { continue; }

        std::vector<lt::alert*> alerts;
        m_session->pop_alerts(&alerts);

        for (lt::alert *a : alerts)
        {
            lt::torrent_paused_alert *tp = lt::alert_cast<lt::torrent_paused_alert>(a);

            if (tp)
            {
                ++numPaused;
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++numFailed;
                --numOutstandingResumeData;
                continue;
            }

            lt::save_resume_data_alert *rd = lt::alert_cast<lt::save_resume_data_alert>(a);
            if (!rd) { continue; }
            --numOutstandingResumeData;
            if (!rd->resume_data) { continue; }

            // PicoTorrent state
            rd->resume_data->dict().insert({ "pT-queuePosition", rd->handle.status().queue_position });

            std::vector<char> buf;
            lt::bencode(std::back_inserter(buf), *rd->resume_data);

            std::string info_hash = lt::to_hex(rd->handle.info_hash().to_string());
            std::string file_name = info_hash + ".dat";
            std::wstring dat_file = IO::Path::Combine(torrents_dir, TWS(file_name));

            std::error_code ec;
            IO::File::WriteAllBytes(dat_file, buf, ec);
        }
    }
}

void CMainFrame::OnAlertNotify()
{
    PostMessage(PT_ALERT);
}

void CMainFrame::OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Controllers::AddTorrentController atc(m_session);
    atc.Execute();
}

void CMainFrame::OnFileAddMagnetLink(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Dialogs::AddMagnetLinkDialog dlg;
    if (dlg.DoModal() == IDOK)
    {
        std::vector<lt::torrent_info> ti = dlg.GetTorrentFiles();

        Controllers::AddTorrentController atc(m_session);
        atc.Execute(ti);
    }
}

void CMainFrame::OnViewPreferences(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Controllers::ViewPreferencesController ctrl(m_session);
    ctrl.Execute();
}

void CMainFrame::OnHelpAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Dialogs::AboutDialog dlg;
    dlg.DoModal();
}

LRESULT CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    // Create the UI
    ResizeClient(SX(800), SY(200));
    SetIcon(LoadIcon(lpCreateStruct->hInstance, MAKEINTRESOURCE(IDI_APPICON)));
    SetMenu(UI::MainMenu::Create());
    SetWindowText(TEXT("PicoTorrent"));

    // NotifyIcon
    m_notifyIcon = std::make_shared<UI::NotifyIcon>(m_hWnd);
    m_notifyIcon->Create();

    // Torrent list view
    CListViewCtrl list;
    list.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT);

    m_torrentList = std::make_shared<UI::TorrentListView>(list);
    m_hWndClient = *m_torrentList;

    // Set our status bar
    m_statusBar = std::make_shared<UI::StatusBar>();
    m_hWndStatusBar = m_statusBar->Create(m_hWnd, rcDefault);

    // Create session
    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::category_t::all_categories);

    m_session = std::make_shared<lt::session>(settings);
    m_session->add_dht_router({ "router.bittorrent.com", 6881 });
    m_session->add_dht_router({ "router.utorrent.com", 6881 });
    m_session->add_dht_router({ "dht.transmissionbt.com", 6881 });
    m_session->add_dht_router({ "dht.aelitis.com", 6881 }); // Vuze

    LoadState();
    LoadTorrents();

    m_session->set_alert_notify(std::bind(&CMainFrame::OnAlertNotify, this));

    // Set the timer which updates every second
    SetTimer(6060, 1000);

    return 0;
}

void CMainFrame::OnDestroy()
{
    typedef boost::function<void()> notify_func_t;
    m_session->set_alert_notify(notify_func_t());

    SaveState();
    SaveTorrents();

    // Destroy the notify icon
    m_notifyIcon->Destroy();

    PostQuitMessage(0);
}

LRESULT CMainFrame::OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    std::vector<lt::alert*> alerts;
    m_session->pop_alerts(&alerts);

    for (lt::alert* alert : alerts)
    {
        switch (alert->type())
        {
        case lt::add_torrent_alert::alert_type:
        {
            lt::add_torrent_alert* ata = lt::alert_cast<lt::add_torrent_alert>(alert);

            if (ata->error)
            {
                // LOG
                break;
            }

            if (std::find(m_find_metadata.begin(), m_find_metadata.end(), ata->params.info_hash) != m_find_metadata.end())
            {
                // This is a torrent we search for metadata
                break;
            }

            // A check to see if this is a hash we have added from the
            // torrent directory when starting PicoTorrent.
            std::vector<lt::sha1_hash>::iterator h = std::find(
                m_muted_hashes.begin(),
                m_muted_hashes.end(),
                ata->handle.info_hash());

            if (h == m_muted_hashes.end())
            {
                if (ata->handle.torrent_file())
                {
                    std::error_code ec;
                    Core::Torrent::Save(ata->handle.torrent_file(), ec);

                    // Post the torrent resume data so we can save it as well.
                    // Since we load each torrent by the .dat file and not .torrent
                    // file, we need this here.
                    ata->handle.save_resume_data();
                }
                else
                {
                    // Save metadata resume info
                }
            }
            else
            {
                m_muted_hashes.erase(h);
            }

            // Our torrent handle
            m_torrents.insert({ ata->handle.info_hash(), ata->handle });

            // Add a model to our torrent list
            Models::Torrent model = Models::Torrent::Map(ata->handle.status());
            m_torrentList->Add(model);

            break;
        }
        case lt::metadata_received_alert::alert_type:
        {
            lt::metadata_received_alert* mra = lt::alert_cast<lt::metadata_received_alert>(alert);

            if (std::find(m_find_metadata.begin(), m_find_metadata.end(), mra->handle.info_hash()) != m_find_metadata.end())
            {
                const lt::torrent_info* ti = mra->handle.torrent_file().get();

                for (HWND hWndListener : m_listeners)
                {
                    ::SendMessage(hWndListener, PT_METADATAFOUND, NULL, reinterpret_cast<LPARAM>(ti));
                }

                m_session->remove_torrent(mra->handle, lt::session::options_t::delete_files);
                break;
            }

            std::error_code ec;
            Core::Torrent::Save(mra->handle.torrent_file(), ec);

            mra->handle.save_resume_data();
            break;
        }
        case lt::save_resume_data_alert::alert_type:
        {
            // Save resume data to the .dat file
            lt::save_resume_data_alert* srda = lt::alert_cast<lt::save_resume_data_alert>(alert);
            
            // Insert PicoTorrent-specific data
            srda->resume_data->dict().insert({ "pT-queuePosition", srda->handle.status().queue_position });
            // ... and more

            std::vector<char> buf;
            lt::bencode(std::back_inserter(buf), *srda->resume_data);

            std::wstring torrents_dir = IO::Path::Combine(Environment::GetDataPath(), TEXT("Torrents"));
            if (!IO::Directory::Exists(torrents_dir)) { IO::Directory::Create(torrents_dir); }

            std::string hash = lt::to_hex(srda->handle.info_hash().to_string());
            std::string file_name = hash + ".dat";

            std::wstring dat_file = IO::Path::Combine(torrents_dir, TWS(file_name));
            std::error_code ec;
            IO::File::WriteAllBytes(dat_file, buf, ec);

            break;
        }
        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
            // ssa->
            break;
        }
        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);
            if (sua->status.empty()) { break; }

            int dl_rate = 0;
            int64_t dl_done = 0;
            int64_t dl_wanted = 0;

            for (lt::torrent_status& ts : sua->status)
            {
                if (std::find(m_find_metadata.begin(), m_find_metadata.end(), ts.info_hash) != m_find_metadata.end())
                {
                    continue;
                }

                dl_done += ts.total_wanted_done;
                dl_wanted += ts.total_wanted;

                Models::Torrent model = Models::Torrent::Map(ts);
                m_torrentList->Update(model);

                for (HWND hWndListener : m_listeners)
                {
                    ::SendMessage(hWndListener, PT_TORRENT_UPDATED, NULL, (LPARAM)&ts.info_hash);
                }

                dl_rate += ts.download_payload_rate;
            }

            // TODO:
            // Instead of looping through each torrent,
            // we should do something smart with the torrents
            // we are already looping through.
            if (m_taskbar != nullptr)
            {
                m_taskbar->SetProgressState(TBPF_NORMAL);
                m_taskbar->SetProgressValue(dl_done, dl_wanted);
            }

            std::pair<int, int> indices = m_torrentList->GetVisibleIndices();
            m_torrentList->RedrawItems(indices.first, indices.second);

            break;
        }
        case lt::torrent_removed_alert::alert_type:
        {
            lt::torrent_removed_alert* tra = lt::alert_cast<lt::torrent_removed_alert>(alert);

            auto f = std::find(m_find_metadata.begin(), m_find_metadata.end(), tra->info_hash);
            if (f != m_find_metadata.end())
            {
                m_find_metadata.erase(f);
                break;
            }

            Models::Torrent t = Models::Torrent::Map(tra->info_hash);
            m_torrentList->Remove(t);

            // Remove from m_torrents
            m_torrents.erase(tra->info_hash);
            break;
        }
        }
    }

    return 0;
}

void CMainFrame::OnTimerElapsed(UINT_PTR nIDEvent)
{
    m_session->post_session_stats();
    m_session->post_torrent_updates();
}
