#include "CMainFrame.hpp"

#include <algorithm>
#include <strsafe.h>

#include <libtorrent/hex.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include "API/PicoTorrent.hpp"
#include "CommandLine.hpp"
#include "Commands/FindMetadataCommand.hpp"
#include "Commands/InvokeCommand.hpp"
#include "Commands/MoveTorrentsCommand.hpp"
#include "Commands/PauseTorrentsCommand.hpp"
#include "Commands/QueueTorrentCommand.hpp"
#include "Commands/RemoveTorrentsCommand.hpp"
#include "Commands/ResumeTorrentsCommand.hpp"
#include "Commands/ShowTorrentDetailsCommand.hpp"
#include "Configuration.hpp"
#include "Core/SessionLoader.hpp"
#include "Core/SessionUnloader.hpp"
#include "Core/Torrent.hpp"
#include "Controllers/AddMagnetLinkController.hpp"
#include "Controllers/AddTorrentController.hpp"
#include "Controllers/NotifyIconController.hpp"
#include "Controllers/RemoveTorrentsController.hpp"
#include "Controllers/TorrentDetailsController.hpp"
#include "Controllers/ViewPreferencesController.hpp"
#include "Dialogs/AboutDialog.hpp"
#include "Dialogs/OpenFileDialog.hpp"
#include "Environment.hpp"
#include "Log.hpp"
#include "IO/Directory.hpp"
#include "IO/File.hpp"
#include "IO/Path.hpp"
#include "Mapping/TorrentMapper.hpp"
#include "Scaler.hpp"
#include "SleepManager.hpp"
#include "StringUtils.hpp"
#include "Translator.hpp"
#include "UI/MainMenu.hpp"
#include "UI/NotifyIcon.hpp"
#include "UI/StatusBar.hpp"
#include "UI/Taskbar.hpp"
#include "UI/TorrentListView.hpp"
#include "UIState.hpp"
#include "VersionInformation.hpp"

namespace lt = libtorrent;

const UINT CMainFrame::TaskbarButtonCreated = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));

CMainFrame::CMainFrame()
    : m_metrics(lt::session_stats_metrics()),
    m_threadId(std::this_thread::get_id())
{
    m_mutex = CreateMutex(NULL, FALSE, TEXT("PicoTorrent/1.0"));
    m_singleInstance = GetLastError() != ERROR_ALREADY_EXISTS;
}

CMainFrame::~CMainFrame()
{
    if (m_mutex) { CloseHandle(m_mutex); }
}

void CMainFrame::ActivateOtherInstance(LPTSTR lpstrCmdLine)
{
    HWND hWndOther = FindWindow(TEXT("PicoTorrent/MainFrame"), NULL);

    COPYDATASTRUCT cds;
    std::wstring args = lpstrCmdLine;

    if (!args.empty())
    {
        cds.cbData = (DWORD)(sizeof(wchar_t) * (args.size() + 1));
        cds.dwData = 1;
        cds.lpData = (PVOID)&args[0];
    }

    // Activate other window
    ::SetForegroundWindow(hWndOther);
    ::ShowWindow(hWndOther, SW_RESTORE);
    ::SendMessage(hWndOther, WM_COPYDATA, NULL, (LPARAM)&cds);
}

bool CMainFrame::IsSingleInstance()
{
    return m_singleInstance;
}

void CMainFrame::HandleCommandLine(const CommandLine& cmd)
{
    if (!cmd.files.empty() && cmd.magnet_links.empty())
    {
        Controllers::AddTorrentController atc(m_hWnd, m_session);
        atc.Execute(cmd.files);
    }
    else if (!cmd.magnet_links.empty() && cmd.files.empty())
    {
        Controllers::AddMagnetLinkController amlc(m_hWnd, m_session);
        amlc.Execute(cmd.magnet_links);
    }
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
    return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}

void CMainFrame::Show(int nCmdShow)
{
    Configuration& cfg = Configuration::GetInstance();
    UINT pos = nCmdShow;

    switch (cfg.GetStartupPosition())
    {
    case Configuration::StartupPosition::Hidden:
        pos = SW_HIDE;
        break;
    case Configuration::StartupPosition::Minimized:
        pos = SW_SHOWMINIMIZED;
        break;
    }

    auto state = UIState::GetInstance().GetWindowState("main");

    if (state)
    {
        WINDOWPLACEMENT winplace = { sizeof(WINDOWPLACEMENT) };
        winplace.flags = state->flags;
        winplace.ptMaxPosition.x = state->max_x;
        winplace.ptMaxPosition.y = state->max_y;
        winplace.ptMinPosition.x = state->min_x;
        winplace.ptMinPosition.y = state->min_y;
        winplace.rcNormalPosition.bottom = state->pos_bottom;
        winplace.rcNormalPosition.left = state->pos_left;
        winplace.rcNormalPosition.right = state->pos_right;
        winplace.rcNormalPosition.top = state->pos_top;
        winplace.showCmd = pos;

        SetWindowPlacement(&winplace);
    }
    else
    {
        ShowWindow(pos);
    }
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
    std::stringstream hex;
    hex << p.info_hash;
    p.save_path = TS(IO::Path::Combine(temp, TWS(hex.str())));

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
        libtorrent::sha1_hash hash;
        std::stringstream ss(t.infoHash);
        ss >> hash;

        const lt::torrent_handle& th = m_torrents.at(hash);
        th.move_storage(ToString(paths[0]));
    }

    return FALSE;
}

LRESULT CMainFrame::OnPauseTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto ptc = reinterpret_cast<Commands::PauseTorrentsCommand*>(lParam);

    for (auto& t : ptc->torrents)
    {
		libtorrent::sha1_hash hash;
        std::stringstream ss(t.infoHash);
        ss >> hash;

        const lt::torrent_handle& th = m_torrents.at(hash);

        th.auto_managed(false);
        th.pause();
    }

    return FALSE;
}

LRESULT CMainFrame::OnQueueTorrent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto qtc = reinterpret_cast<Commands::QueueTorrentCommand*>(lParam);

    libtorrent::sha1_hash hash;
    std::stringstream ss(qtc->torrent.infoHash);
    ss >> hash;

    const lt::torrent_handle& th = m_torrents.at(hash);

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

    Controllers::RemoveTorrentsController rem(m_hWnd, m_session, m_torrents);
    rem.Execute(rm->torrents, rm->removeData);

    return FALSE;
}

void CMainFrame::OnRemoveTorrentsAccelerator(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    std::vector<Torrent> selection = m_torrentList->GetSelectedTorrents();
    bool removeData = nID == IDA_REMOVE_TORRENTS_DATA;

    Controllers::RemoveTorrentsController rem(m_hWnd, m_session, m_torrents);
    rem.Execute(selection, removeData);
}

LRESULT CMainFrame::OnResumeTorrents(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto res = reinterpret_cast<Commands::ResumeTorrentsCommand*>(lParam);

    for (auto& t : res->torrents)
    {
		libtorrent::sha1_hash hash;
        std::stringstream ss(t.infoHash);
        ss >> hash;

        const lt::torrent_handle& th = m_torrents.at(hash);
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

	libtorrent::sha1_hash hash;
	std::stringstream ss(show->torrent.infoHash);
	ss >> hash;

    const lt::torrent_handle& th = m_torrents.at(hash);

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

LRESULT CMainFrame::OnInvoke(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto cmd = reinterpret_cast<Commands::InvokeCommand*>(lParam);
    cmd->callback();

    return FALSE;
}

void CMainFrame::OnSysCommand(UINT nID, CPoint point)
{
    switch (nID)
    {
    case SC_MINIMIZE:
    {
        Configuration& cfg = Configuration::GetInstance();

        if (cfg.UI()->GetShowInNotificationArea()
            && cfg.UI()->GetMinimizeToNotificationArea())
        {
            ShowWindow(SW_HIDE);
            return;
        }

        break;
    }
    }

    SetMsgHandled(FALSE);
}

LRESULT CMainFrame::OnTaskbarButtonCreated(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_taskbar = std::make_shared<UI::Taskbar>(m_hWnd);
    return FALSE;
}

void CMainFrame::OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Controllers::AddTorrentController atc(m_hWnd, m_session);
    atc.Execute();
}

void CMainFrame::OnFileAddMagnetLink(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Controllers::AddMagnetLinkController amlc(m_hWnd, m_session);
    amlc.Execute();
}

void CMainFrame::OnFileExit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    DestroyWindow();
}

void CMainFrame::OnViewPreferences(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Controllers::ViewPreferencesController ctrl(m_notifyIcon, m_session);
    ctrl.Execute();
}

void CMainFrame::OnHelpAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Dialogs::AboutDialog dlg;
    dlg.DoModal();
}

void CMainFrame::OnClose()
{
    Configuration& cfg = Configuration::GetInstance();

    if (cfg.UI()->GetShowInNotificationArea()
        && cfg.UI()->GetCloseToNotificationArea())
    {
        ShowWindow(SW_HIDE);
    }
    else
    {
        SetMsgHandled(FALSE);
    }
}

BOOL CMainFrame::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
    wchar_t *ptr = reinterpret_cast<wchar_t*>(pCopyDataStruct->lpData);
    std::wstring cmd = ptr;

    CommandLine cmdLine = CommandLine::Parse(cmd);
    HandleCommandLine(cmdLine);
    
    return FALSE;
}

LRESULT CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    // Create the UI
    ResizeClient(SX(800), SY(200));
    SetMenu(UI::MainMenu::Create());
    SetWindowText(TEXT("PicoTorrent"));

    Configuration& cfg = Configuration::GetInstance();

    // NotifyIcon
    m_notifyIcon = std::make_shared<UI::NotifyIcon>(m_hWnd);

    if (cfg.UI()->GetShowInNotificationArea())
    {
        m_notifyIcon->Show();
    }

    // Torrent list view
    CListViewCtrl list;
    list.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT);
    m_torrentList = std::make_shared<UI::TorrentListView>(list);
    m_hWndClient = *m_torrentList;

    // Set our status bar
    m_statusBar = std::make_shared<UI::StatusBar>();
    m_hWndStatusBar = m_statusBar->Create(m_hWnd, rcDefault);

    m_sleepManager = std::make_shared<SleepManager>();

    // Load session
    Core::SessionLoader::State state = Core::SessionLoader::Load();
    m_muted_hashes = state.muted_hashes;
    m_session = state.session;
    m_session->set_alert_notify([this]() { PostMessage(PT_ALERT); });

    // Load plugins
    m_api = std::make_shared<API::PicoTorrent>(m_hWnd, m_session);

    std::wstring app_dir = Environment::GetApplicationPath();
    for (std::wstring dll : IO::Directory::GetFiles(app_dir, TEXT("*.dll")))
    {
        typedef bool(*init_fn)(int, std::shared_ptr<IPicoTorrent>);
        // Find the "pico_init_plugin" export of each DLL
        HMODULE hPluginModule = LoadLibrary(dll.c_str());
        if (hPluginModule == NULL) { continue; }

        init_fn init = reinterpret_cast<init_fn>(GetProcAddress(hPluginModule, "pico_init_plugin"));

        if (init == nullptr || init == NULL)
        {
            FreeLibrary(hPluginModule);
            continue;
        }

        if (!init(PICOTORRENT_API_VERSION, m_api))
        {
            FreeLibrary(hPluginModule);
            continue;
        }
    }
    // ------------

    // Set the timer which updates every second
    SetTimer(6060, 1000);

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->AddMessageFilter(this);

    return 0;
}

void CMainFrame::OnDestroy()
{
    WINDOWPLACEMENT wndplace = { sizeof(WINDOWPLACEMENT) };
    if (GetWindowPlacement(&wndplace))
    {
        UIState::WindowState ws;
        ws.flags = wndplace.flags;
        ws.max_x = wndplace.ptMaxPosition.x;
        ws.max_y = wndplace.ptMaxPosition.y;
        ws.min_x = wndplace.ptMinPosition.x;
        ws.min_y = wndplace.ptMinPosition.y;
        ws.pos_bottom = wndplace.rcNormalPosition.bottom;
        ws.pos_left = wndplace.rcNormalPosition.left;
        ws.pos_right = wndplace.rcNormalPosition.right;
        ws.pos_top = wndplace.rcNormalPosition.top;
        ws.show = wndplace.showCmd;

        UIState::GetInstance().SetWindowState("main", ws);
    }

    CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->RemoveMessageFilter(this);

    typedef std::function<void()> notify_func_t;
    m_session->set_alert_notify(notify_func_t());

    Core::SessionUnloader::Unload(m_session);
    PostQuitMessage(0);
}

void CMainFrame::OnSelectAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    m_torrentList->SelectAll();
}

LRESULT CMainFrame::OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
            Torrent model = Mapping::TorrentMapper::Map(ata->handle.status());
            m_torrentList->Add(model);

            // Update status bar
            m_statusBar->SetTorrentCount((int)m_torrents.size(), 0);

            m_api->EmitTorrentAdded(model);

            break;
        }
        case lt::dht_stats_alert::alert_type:
        {
            lt::dht_stats_alert* dsa = lt::alert_cast<lt::dht_stats_alert>(alert);
            // dsa->
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

            std::stringstream hex;
            hex << srda->handle.info_hash();
            std::string file_name = hex.str() + ".dat";

            std::wstring dat_file = IO::Path::Combine(torrents_dir, TWS(file_name));
            std::error_code ec;
            IO::File::WriteAllBytes(dat_file, buf, ec);

            break;
        }
        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert* ssa = lt::alert_cast<lt::session_stats_alert>(alert);
            
            int dht_nodes_idx = lt::find_metric_idx("dht.dht_peers");
            uint64_t dht_nodes = ssa->values[dht_nodes_idx];

            // m_statusBar->SetDhtNodes((int)dht_nodes);
            break;
        }
        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* sua = lt::alert_cast<lt::state_update_alert>(alert);
            if (sua->status.empty()) { break; }

            int dl_rate = 0;
            int ul_rate = 0;
            float dl_progress = 0;
            int currently_downloading = 0;
            bool has_error = false;

			std::vector<Torrent> updatedTorrents;

            for (lt::torrent_status& ts : sua->status)
            {
                if (std::find(m_find_metadata.begin(), m_find_metadata.end(), ts.info_hash) != m_find_metadata.end())
                {
                    continue;
                }

                Torrent model = Mapping::TorrentMapper::Map(ts);
                m_torrentList->Update(model);
                updatedTorrents.push_back(model);

                dl_rate += model.downloadRate;
                ul_rate += model.uploadRate;

                if (model.state == Torrent::State::Error)
                {
                    has_error = true;
                }

                // These are the states we consider 'active' or 'currently downloading'
                // which the taskbar progress value is updated against.
                if (model.state == Torrent::State::Downloading
                    || model.state == Torrent::State::DownloadingChecking
                    || model.state == Torrent::State::DownloadingForced
                    || model.state == Torrent::State::DownloadingStalled)
                {
                    currently_downloading += 1;
                    dl_progress += ts.progress;
                }

                for (HWND hWndListener : m_listeners)
                {
                    ::SendMessage(hWndListener, PT_TORRENT_UPDATED, NULL, (LPARAM)&ts.info_hash);
                }
            }

            if (m_taskbar != nullptr)
            {
                if (dl_progress > 0)
                {
                    m_taskbar->SetProgressState(has_error ? TBPF_ERROR : TBPF_NORMAL);
                    m_taskbar->SetProgressValue(
                        static_cast<uint64_t>(dl_progress * 1000),
                        currently_downloading * 1000);
                }
                else
                {
                    m_taskbar->SetProgressState(TBPF_NOPROGRESS);
                }
            }

            if (currently_downloading > 0)
            {
                m_sleepManager->PreventSleep();
            }
            else
            {
                m_sleepManager->AllowSleep();
            }

            m_statusBar->SetTransferRates(dl_rate, ul_rate);

            std::pair<int, int> indices = m_torrentList->GetVisibleIndices();
            m_torrentList->RedrawItems(indices.first, indices.second);

            // Emit to API
            m_api->EmitTorrentUpdated(updatedTorrents);

            break;
        }
        case lt::torrent_finished_alert::alert_type:
        {
            lt::torrent_finished_alert* tfa = lt::alert_cast<lt::torrent_finished_alert>(alert);
            const lt::torrent_status& ts = tfa->handle.status();

            // Only do this if we have downloaded any payload bytes
            if (ts.total_payload_download <= 0)
            {
                break;
            }

            Configuration& cfg = Configuration::GetInstance();
            bool shouldMove = cfg.GetMoveCompletedDownloads();
            bool onlyFromDefault = cfg.GetMoveCompletedDownloadsFromDefaultOnly();
            std::string targetPath = cfg.GetMoveCompletedDownloadsPath();

            if (shouldMove)
            {
                if (onlyFromDefault && ts.save_path != cfg.GetDefaultSavePath())
                {
                    break;
                }

                tfa->handle.move_storage(targetPath);
            }

            // Emit API event
            m_api->EmitTorrentFinished(Mapping::TorrentMapper::Map(ts));

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

			std::stringstream hex;
			hex << tra->info_hash;

            m_torrentList->Remove(hex.str());
            m_torrents.erase(tra->info_hash);

            // Remove the torrent and dat file from the hard drive
            std::wstring hash = TWS(hex.str());
            std::wstring torrents_dir = IO::Path::Combine(Environment::GetDataPath(), TEXT("Torrents"));
            std::wstring torrent_file = IO::Path::Combine(torrents_dir, hash + L".torrent");
            std::wstring torrent_dat = IO::Path::Combine(torrents_dir, hash + L".dat");

            if (IO::File::Exists(torrent_file)) { IO::File::Delete(torrent_file); }
            if (IO::File::Exists(torrent_dat)) { IO::File::Delete(torrent_dat); }

            m_api->EmitTorrentRemoved(tra->info_hash);
            m_statusBar->SetTorrentCount((int)m_torrents.size(), 0);

            break;
        }
        }
    }

    return 0;
}

void CMainFrame::OnTimerElapsed(UINT_PTR nIDEvent)
{
    m_session->post_dht_stats();
    m_session->post_session_stats();
    m_session->post_torrent_updates();
}

void CMainFrame::OnUnhandledCommand(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    /*if (m_extensionMenuItems.find(nID) != m_extensionMenuItems.end())
    {
        m_extensionMenuItems.at(nID)->OnClick();
    }*/
}
