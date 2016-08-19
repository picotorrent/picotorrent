#include "CMainFrame.hpp"

#include <fstream>
#include <queue>
#include <strsafe.h>

#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include "Environment.hpp"
#include "Scaler.hpp"
#include "StringUtils.hpp"
#include "Translator.hpp"
#include "Controllers/AddTorrentController.hpp"
#include "Controllers/ViewPreferencesController.hpp"
#include "Core/Torrent.hpp"
#include "Dialogs/AboutDialog.hpp"
#include "IO/Directory.hpp"
#include "IO/File.hpp"
#include "IO/Path.hpp"
#include "UI/MainMenu.hpp"
#include "UI/StatusBar.hpp"
#include "UI/TorrentListView.hpp"

#define LV_COL_NAME 1
#define LV_COL_QUEUE_POSITION 2
#define LV_COL_SIZE 3
#define LV_COL_STATUS 4
#define LV_COL_PROGRESS 5
#define LV_COL_ETA 6
#define LV_COL_DL 7
#define LV_COL_UL 8
#define LV_COL_SEEDS 9
#define LV_COL_PEERS 10
#define LV_COL_RATIO 11

namespace lt = libtorrent;

struct SessionLoadItem
{
    SessionLoadItem(const std::wstring &p)
        : path(p)
    {
    }

    std::wstring path;

    std::vector<char> resume_data;
    std::string magnet_uri;
    std::string save_path;
    std::string url;
};

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
    // Load state
    std::wstring data_path = Environment::GetDataPath();
    std::wstring state_file = IO::Path::Combine(data_path, TEXT("Session.dat"));

    if (!IO::File::Exists(state_file))
    {
        return;
    }

    std::error_code ec;
    std::vector<char> buf = IO::File::ReadAllBytes(state_file, ec);

    if (ec)
    {
        // LOG this mother
        return;
    }

    lt::bdecode_node node;
    lt::error_code ltec;
    lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

    if (ltec)
    {
        // LOG this mother
        return;
    }

    m_session->load_state(node);
}

void CMainFrame::LoadTorrents()
{
    std::wstring data_path = Environment::GetDataPath();
    std::wstring torrents_dir = IO::Path::Combine(data_path, TEXT("Torrents"));

    if (!IO::Directory::Exists(torrents_dir))
    {
        // LOG this mother
        return;
    }

    std::vector<std::wstring> torrent_dat_files = IO::Directory::GetFiles(torrents_dir, TEXT("*.dat"));
    // LOG amount of torrents

    typedef std::pair<int64_t, SessionLoadItem> prio_item_t;
    auto comparer = [](const prio_item_t &lhs, const prio_item_t &rhs)
    {
        return lhs.first > rhs.first;
    };

    std::priority_queue<prio_item_t, std::vector<prio_item_t>, decltype(comparer)> queue(comparer);
    int64_t maxPosition = std::numeric_limits<int64_t>::max();

    for (std::wstring& dat_file : torrent_dat_files)
    {
        SessionLoadItem item(dat_file);

        std::error_code ec;
        item.resume_data = IO::File::ReadAllBytes(dat_file, ec);

        if (ec)
        {
            // LOG
            continue;
        }

        lt::error_code ltec;
        lt::bdecode_node node;
        lt::bdecode(
            &item.resume_data[0],
            &item.resume_data[0] + item.resume_data.size(),
            node,
            ltec);

        if (ltec)
        {
            // LOG
            continue;
        }

        if (node.type() != lt::bdecode_node::type_t::dict_t)
        {
            // LOG
            continue;
        }

        item.magnet_uri = node.dict_find_string_value("pT-magnetUri");
        item.save_path = node.dict_find_string_value("pT-savePath" /* config default save path */);
        item.url = node.dict_find_string_value("pT-url");

        int64_t queuePosition = node.dict_find_int_value("pT-queuePosition", maxPosition);
        if (queuePosition < 0) { queuePosition = maxPosition; }

        queue.push({ queuePosition, item });
    }

    while (!queue.empty())
    {
        SessionLoadItem item = queue.top().second;
        queue.pop();

        std::wstring torrent_file = IO::Path::ReplaceExtension(item.path, TEXT(".torrent"));

        if (!IO::File::Exists(torrent_file)
            && item.magnet_uri.empty())
        {
            // LOG
            IO::File::Delete(torrent_file);
            continue;
        }

        lt::add_torrent_params params;

        if (!item.resume_data.empty())
        {
            lt::error_code ltec;
            params = lt::read_resume_data(
                &item.resume_data[0],
                (int)item.resume_data.size(),
                ltec);

            if (ltec)
            {
                // LOG
            }
        }

        if (IO::File::Exists(torrent_file))
        {
            std::error_code ec;
            std::vector<char> torrent_buf = IO::File::ReadAllBytes(torrent_file, ec);

            if (ec)
            {
                // LOG
            }

            lt::bdecode_node node;
            lt::error_code ltec;
            lt::bdecode(
                &torrent_buf[0],
                &torrent_buf[0] + torrent_buf.size(),
                node,
                ltec);

            if (ltec)
            {
                // LOG
                continue;
            }

            params.ti = boost::make_shared<lt::torrent_info>(node);
            m_muted_hashes.push_back(params.ti->info_hash());
        }

        if (params.save_path.empty())
        {
            params.save_path = item.save_path;
        }

        if (item.url.empty())
        {
            params.url = item.magnet_uri;
        }
        else
        {
            params.url = item.url;
        }

        m_session->async_add_torrent(params);
    }
}

void CMainFrame::SaveState()
{
    // Save session state
    lt::entry entry;
    m_session->save_state(entry);

    std::vector<char> buf;
    lt::bencode(std::back_inserter(buf), entry);

    std::wstring data_path = Environment::GetDataPath();
    std::wstring state_file = IO::Path::Combine(data_path, TEXT("Session.dat"));

    if (!IO::Directory::Exists(Environment::GetDataPath()))
    {
        IO::Directory::Create(Environment::GetDataPath());
    }

    std::error_code ec;
    IO::File::WriteAllBytes(state_file, buf, ec);

    if (ec)
    {
        // LOG
    }
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
            // TODO(log)
            continue;
        }

        st.handle.save_resume_data();
        ++numOutstandingResumeData;
    }

    // LOG(info) << "Saving resume data for " << numOutstandingResumeData << " torrent(s)";
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
                // TODO(log)
                continue;
            }

            if (lt::alert_cast<lt::save_resume_data_failed_alert>(a))
            {
                ++numFailed;
                --numOutstandingResumeData;
                // TODO(log)
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
    PostMessage(LT_SESSION_ALERT);
}

void CMainFrame::OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Controllers::AddTorrentController atc(m_session);
    atc.Execute();
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

    // Torrent list view
    CListViewCtrl list;
    list.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT);

    m_torrentList = std::make_shared<UI::TorrentListView>(list, m_hashes, m_torrents);
    m_hWndClient = m_torrentList->GetHandle();

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

    PostQuitMessage(0);
}

LRESULT CMainFrame::OnLVGetItemProgress(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::ListView::GetItemProgress* git = reinterpret_cast<UI::ListView::GetItemProgress*>(lParam);

    if (git->item_index >= m_hashes.size())
    {
        // LOG
        return 0;
    }

    lt::sha1_hash& hash = m_hashes.at(git->item_index);
    lt::torrent_status& ts = m_torrents.at(hash);

    switch (git->column_id)
    {
    case LV_COL_PROGRESS:
    {
        git->progress = ts.progress;
        break;
    }
    }

    return 0;
}

LRESULT CMainFrame::OnLVSetColumnSortOrder(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::ListView::SetColumnSortOrder* scso = reinterpret_cast<UI::ListView::SetColumnSortOrder*>(lParam);

    std::function<bool(const lt::torrent_status& ts1, const lt::torrent_status& ts2)> sort;
    bool is_ascending = (scso->order == UI::ListView::SortOrder::Ascending);

    /*
#define LV_COL_STATUS 4
#define LV_COL_PROGRESS 5
#define LV_COL_ETA 6
#define LV_COL_DL 7
#define LV_COL_UL 8
#define LV_COL_SEEDS 9
#define LV_COL_PEERS 10
#define LV_COL_RATIO 11*/

    switch (scso->column_id)
    {
    case LV_COL_NAME:
    {
        sort = [this, is_ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
        {
            if (is_ascending) { return ts1.name < ts2.name; }
            else { return ts1.name > ts2.name; }
        };
        break;
    }
    case LV_COL_QUEUE_POSITION:
    {
        sort = [this, is_ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
        {
            if (is_ascending) { return ts1.queue_position < ts2.queue_position; }
            else { return ts1.queue_position > ts2.queue_position; }
        };
        break;
    }
    case LV_COL_SIZE:
    {
        sort = [this, is_ascending](const lt::torrent_status& ts1, const lt::torrent_status& ts2)
        {
            int64_t t1s = ts1.handle.torrent_file()->total_size();
            int64_t t2s = ts2.handle.torrent_file()->total_size();

            if (is_ascending) { return t1s < t2s; }
            else { return t1s > t2s; }
        };
        break;
    }
    }

    if (sort)
    {
        scso->did_sort = true;
        std::sort(m_hashes.begin(), m_hashes.end(),
            [this, sort](const lt::sha1_hash& h1, const lt::sha1_hash& h2)
            {
                const lt::torrent_status& ts1 = m_torrents.at(h1);
                const lt::torrent_status& ts2 = m_torrents.at(h2);
                return sort(ts1, ts2);
            });
    }

    return FALSE;
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

            lt::sha1_hash hash = ata->handle.info_hash();

            m_hashes.push_back(hash);
            m_torrents.insert({ hash, ata->handle.status() });
            m_torrentList->SetItemCount((int)m_torrents.size());

            break;
        }
        case lt::metadata_received_alert::alert_type:
        {
            lt::metadata_received_alert* mra = lt::alert_cast<lt::metadata_received_alert>(alert);

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

            for (lt::torrent_status& ts : sua->status)
            {
                m_torrents[ts.info_hash] = ts;
                dl_rate += ts.download_payload_rate;
            }

            OutputDebugStringA(std::to_string(dl_rate).c_str());
            OutputDebugStringA(",");

            std::pair<int, int> indices = m_torrentList->GetVisibleIndices();
            m_torrentList->RedrawItems(indices.first, indices.second);

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
