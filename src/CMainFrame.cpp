#include "CMainFrame.hpp"

#include <fstream>
#include <queue>

#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_status.hpp>

#include "Environment.hpp"
#include "StringUtils.hpp"
#include "Translator.hpp"
#include "Controllers/AddTorrentController.hpp"
#include "IO/Directory.hpp"
#include "IO/File.hpp"
#include "IO/Path.hpp"
#include "UI/ListView.hpp"
#include "UI/MainMenu.hpp"
#include "UI/StatusBar.hpp"

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
            std::wstringstream file_name;
            file_name << ToWideString(info_hash) << TEXT(".dat");

            std::wstring torrents_dir = IO::Path::Combine(Environment::GetDataPath(), TEXT("Torrents"));
            std::wstring torrent_file = IO::Path::Combine(torrents_dir, file_name.str());

            std::error_code ec;
            IO::File::WriteAllBytes(torrent_file, buf, ec);
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

LRESULT CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    // Create the UI
    ResizeClient(800, 200);
    SetMenu(UI::MainMenu());
    SetWindowText(TEXT("PicoTorrent"));

    // Torrent list view
    CListViewCtrl list;
    list.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT);

    m_torrentList = std::make_shared<UI::ListView>(list);
    m_torrentList->AddColumn(LV_COL_NAME, TRW("name"), 280, UI::ListView::ColumnType::Text);
    m_torrentList->AddColumn(LV_COL_QUEUE_POSITION, TRW("queue_position"), 30, UI::ListView::ColumnType::Number);
    m_torrentList->AddColumn(LV_COL_SIZE, TRW("size"), 80, UI::ListView::ColumnType::Number);
    m_torrentList->AddColumn(LV_COL_STATUS, TRW("status"), 120, UI::ListView::ColumnType::Text);
    m_torrentList->AddColumn(LV_COL_PROGRESS, TRW("progress"), 100, UI::ListView::ColumnType::Progress);
    m_torrentList->AddColumn(LV_COL_ETA, TRW("eta"), 80, UI::ListView::ColumnType::Number);
    m_torrentList->AddColumn(LV_COL_DL, TRW("dl"), 80, UI::ListView::ColumnType::Number);
    m_torrentList->AddColumn(LV_COL_UL, TRW("ul"), 80, UI::ListView::ColumnType::Number);
    m_torrentList->AddColumn(LV_COL_RATIO, TRW("ratio"), 80, UI::ListView::ColumnType::Number);
    m_torrentList->AddColumn(LV_COL_SEEDS, TRW("seeds"), 80, UI::ListView::ColumnType::Number);
    m_torrentList->AddColumn(LV_COL_PEERS, TRW("peers"), 80, UI::ListView::ColumnType::Number);

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

    m_session->set_alert_notify(std::bind(&CMainFrame::OnAlertNotify, this));
    // TODO: m_session->set_load_function()

    return 0;
}

void CMainFrame::OnDestroy()
{
    typedef boost::function<void()> notify_func_t;
    m_session->set_alert_notify(notify_func_t());

    SaveState();

    PostQuitMessage(0);
}

LRESULT CMainFrame::OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return 0;
}
