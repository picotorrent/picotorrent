#include "mainwindow.h"

#include <libtorrent/alert_types.hpp>

#include "aboutdialog.h"

typedef boost::function<void()> notify_func_t;
namespace lt = libtorrent;

CMainWindow::CMainWindow()
{
    session_ = std::make_unique<libtorrent::session>(lt::settings_pack(), 0);
    session_->set_alert_notify(std::bind(&CMainWindow::OnSessionAlertNotify, this));
    sessionMetrics_ = lt::session_stats_metrics();
}

CMainWindow::~CMainWindow()
{
}

LRESULT CMainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::all_categories);
    settings.set_str(lt::settings_pack::listen_interfaces, "0.0.0.0:6881");

    session_->apply_settings(settings);

    m_hWndClient = torrentList_.Create(m_hWnd,
        rcDefault,
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_AUTOARRANGE | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS,
        WS_EX_CLIENTEDGE);

    torrentList_.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

    torrentList_.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 250, 0);
    torrentList_.InsertColumn(1, _T("#"), LVCFMT_RIGHT, 40, 1);
    torrentList_.InsertColumn(2, _T("Size"), LVCFMT_RIGHT, 100, 2);
    torrentList_.InsertColumn(3, _T("Status"), LVCFMT_LEFT, 140, 3);
    torrentList_.InsertColumn(4, _T("DL"), LVCFMT_RIGHT, 100, 4);
    torrentList_.InsertColumn(5, _T("UL"), LVCFMT_RIGHT, 100, 5);

    // Set up our timer. Responsible for checking for updates, refreshing torrents, etc.
    SetTimer(1, 1000, NULL);

    return 0;
}

LRESULT CMainWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    bHandled = FALSE;
    return 1;
}

LRESULT CMainWindow::OnFileAddTorrent(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    LPCTSTR files =
        L"Torrent Files (*.torrent)\0*.torrent\0"
        L"All Files (*.*)\0*.*\0\0";

    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, files);

    if (dlg.DoModal() == IDOK)
    {
        std::string fileName = pt::to_string(dlg.m_szFileName);

        libtorrent::add_torrent_params p;
        p.save_path = "C:/Downloads";
        p.ti = boost::make_shared<libtorrent::torrent_info>(fileName);

        session_->async_add_torrent(p);
    }

    return 0;
}

LRESULT CMainWindow::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    PostMessage(WM_CLOSE);
    return 0;
}

LRESULT CMainWindow::OnHelpAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CAboutDialog about;
    about.DoModal();

    return 0;
}

LRESULT CMainWindow::OnSessionAlert(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    std::vector<lt::alert*> alerts;
    session_->pop_alerts(&alerts);

    for (lt::alert* alert : alerts)
    {
        switch (alert->type())
        {
        case libtorrent::torrent_added_alert::alert_type:
        {
            libtorrent::torrent_added_alert* al = libtorrent::alert_cast<libtorrent::torrent_added_alert>(alert);
            lt::torrent_status status = al->handle.status();

            int item = torrentList_.AddItem(torrentList_.GetItemCount(), 0, pt::to_lpwstr(status.name));
            torrentList_.AddItem(item, 1, pt::to_lpwstr(std::to_string(status.queue_position)));
            
            if (al->handle.torrent_file())
            {
                torrentList_.AddItem(item, 2, pt::to_lpwstr(std::to_string(al->handle.torrent_file()->total_size())));
            }
            else
            {
                torrentList_.AddItem(item, 2, L"");
            }

            torrentList_.AddItem(item, 3, pt::to_lpwstr(std::to_string(status.state)));
            torrentList_.AddItem(item, 4, pt::to_lpwstr(std::to_string(status.download_rate)));
            torrentList_.AddItem(item, 5, pt::to_lpwstr(std::to_string(status.upload_rate)));
        }
        
        case lt::metadata_received_alert::alert_type:
        {
            lt::metadata_received_alert* al = lt::alert_cast<lt::metadata_received_alert>(alert);
        }
        break;

        case lt::session_stats_alert::alert_type:
        {
            lt::session_stats_alert* al = lt::alert_cast<lt::session_stats_alert>(alert);
            // Do stuff
        }
        break;

        case lt::state_update_alert::alert_type:
        {
            lt::state_update_alert* al = lt::alert_cast<lt::state_update_alert>(alert);
            uint64_t dl = 0;
            uint64_t ul = 0;

            for (lt::torrent_status status : al->status)
            {
                // Update items
                dl += status.download_payload_rate;
                ul += status.upload_payload_rate;
            }

            char buffer[100];
            snprintf(buffer, sizeof(buffer), "PicoTorrent [DL:%s/s] [UL:%s/s]", pt::to_file_size(dl).c_str(), pt::to_file_size(ul).c_str());
            SetWindowText(pt::to_lpwstr(buffer));
        }
        break;

        default:
            break;
        }
    }

    return 0;
}

LRESULT CMainWindow::OnBackgroundTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    session_->post_dht_stats();
    session_->post_session_stats();
    session_->post_torrent_updates();

    return 0;
}

LRESULT CMainWindow::OnTorrentItemDoubleClick(int idCtrl, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    if (pnmh->hwndFrom != torrentList_.m_hWnd)
    {
        return 0;
    }

    POINT pt;
    ::GetCursorPos((LPPOINT)&pt);
    torrentList_.ScreenToClient(&pt);

    LVHITTESTINFO htInfo;
    htInfo.pt = pt;
    torrentList_.HitTest(&htInfo);

    if (!(htInfo.flags & LVHT_ONITEM))
    {
        return 0;
    }

    LVITEM item;
    item.mask = LVIF_PARAM;
    item.iItem = htInfo.iItem;
    item.iSubItem = 0;

    if (!torrentList_.GetItem(&item))
    {
        return 0;
    }

    libtorrent::sha1_hash* hash = (libtorrent::sha1_hash*)item.lParam;
    std::string encodedHash = libtorrent::to_hex(hash->to_string());

    // If we have this hash in our details map, show the window.
    // Otherwise, create a new window and connect it with the hash.

    if (torrentDetails_.find(encodedHash) == torrentDetails_.end())
    {
        // TODO
        /*std::shared_ptr<CTorrentDetailsFrame> details = std::make_shared<CTorrentDetailsFrame>();
        details->CreateEx(m_hWnd);
        details->ShowWindow(SW_SHOWNORMAL);

        torrentDetails_[encodedHash] = details;*/
    }
    else
    {
        //torrentDetails_[encodedHash]->BringWindowToTop();
    }

    return 0;
}

void CMainWindow::OnSessionAlertNotify()
{
    PostMessage(WM_APP + 0x01);
}
