#include "mainwindow.h"

#include <libtorrent/alert_types.hpp>

#include "aboutdialog.h"

typedef boost::function<void(std::auto_ptr<libtorrent::alert>)> dispatch_function_t;

CMainWindow::CMainWindow()
{
    libtorrent::fingerprint fp("LT", LIBTORRENT_VERSION_MAJOR, LIBTORRENT_VERSION_MINOR, 0, 0);
    session_ = std::make_unique<libtorrent::session>(fp, 0);
}

CMainWindow::~CMainWindow()
{
}

LRESULT CMainWindow::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    session_->set_alert_mask(libtorrent::alert::category_t::all_categories);
    session_->set_alert_dispatch(std::bind(&CMainWindow::AlertDispatch, this, std::placeholders::_1));
    session_->listen_on(std::make_pair(6881, 6889));

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

    return 0;
}

LRESULT CMainWindow::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
    session_->set_alert_dispatch(dispatch_function_t());

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
        libtorrent::add_torrent_params p;
        p.save_path = "C:/Downloads";
        p.ti = new libtorrent::torrent_info(dlg.m_szFileName);

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

LRESULT CMainWindow::OnSessionAlert(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    libtorrent::alert* alert = (libtorrent::alert*)lParam;

    switch (alert->type())
    {
    case libtorrent::torrent_added_alert::alert_type:
    {
        libtorrent::torrent_added_alert* al = libtorrent::alert_cast<libtorrent::torrent_added_alert>(alert);
        
        LVITEM item = { 0 };
        item.lParam = (LPARAM)new libtorrent::sha1_hash(al->handle.info_hash());
        item.mask = LVIF_TEXT | LVIF_PARAM;
        item.pszText = LPSTR_TEXTCALLBACK;
        item.iItem = torrentList_.GetItemCount();
        item.iSubItem = 0;

        int i = torrentList_.InsertItem(&item);
        printf("");
    }
        break;

    default:
        break;
    }

    return 0;
}

LRESULT CMainWindow::OnLVGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmh;

    if (plvdi == nullptr)
    {
        return 0;
    }

    libtorrent::sha1_hash* hash = (libtorrent::sha1_hash*)plvdi->item.lParam;
    libtorrent::torrent_handle handle = session_->find_torrent(*hash);

    if (!handle.is_valid())
    {
        // Log
        return 0;
    }

    libtorrent::torrent_status status = handle.status();

    switch (plvdi->item.iSubItem)
    {
    case 0: // Name
        plvdi->item.pszText = pt::to_lpwstr(status.name);
        break;
    case 1: // Queue position
        plvdi->item.pszText = pt::to_lpwstr(std::to_string(status.queue_position));
        break;
    case 2: // Size
    {
        size_t size = -1;

        if (handle.torrent_file())
        {
            size = handle.torrent_file()->total_size();
        }

        plvdi->item.pszText = pt::to_lpwstr(std::to_string(size));
    }
        break;
    case 3: // status
        plvdi->item.pszText = pt::to_lpwstr(std::to_string(status.state));
        break;
    case 4: // DL
        plvdi->item.pszText = pt::to_lpwstr(std::to_string(status.download_rate));
        break;
    case 5: // UL
        plvdi->item.pszText = pt::to_lpwstr(std::to_string(status.upload_rate));
        break;
    }

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

void CMainWindow::AlertDispatch(std::auto_ptr<libtorrent::alert> alert)
{
    PostMessage(WM_APP + 0x01, NULL, (LPARAM)alert.release());
}
