#include "mainframe.h"

#pragma warning(disable: 4005 4245 4267 4800)
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session_handle.hpp>
#include <libtorrent/torrent_handle.hpp>
#pragma warning(default: 4005 4245 4267 4800)
#include <shellapi.h>

#include "addtorrentdialog.h"
#include "scaler.h"
#include "../commandline.h"
#include "../path.h"
#include "../util.h"
#include "../controllers/addtorrentcontroller.h"
#include "../io/file.h"

namespace fs = boost::filesystem;
namespace lt = libtorrent;
using namespace pico;

MainFrame::MainFrame(lt::session_handle& session)
    : session_(session)
{
}

void MainFrame::AddTorrent(lt::torrent_status& status)
{
    std::lock_guard<std::mutex> lock(mtx_);
    AddTorrentUnsafe(status);
    UpdateTorrentUnsafe(status);
}

void MainFrame::UpdateTorrent(lt::torrent_status& status)
{
    std::lock_guard<std::mutex> lock(mtx_);
    UpdateTorrentUnsafe(status);
}

void MainFrame::RemoveTorrent(lt::sha1_hash& hash)
{
    std::lock_guard<std::mutex> lock(mtx_);
    RemoveTorrentUnsafe(hash);
}

void MainFrame::AddTorrentUnsafe(lt::torrent_status& status)
{
    LVITEM item = { 0 };
    item.iItem = torrentsList_.GetItemCount();
    item.lParam = (LPARAM)new lt::sha1_hash(status.info_hash);
    item.mask = LVIF_PARAM | LVFIF_TEXT;
    item.pszText = L"";

    torrents_[status.info_hash] = item.iItem;
    torrentsList_.InsertItem(&item);
}

void MainFrame::RemoveTorrentUnsafe(lt::sha1_hash& hash)
{
    int idx = torrents_[hash];

    // Loop through each item that has an index greater
    // than the one we removed, and subtract one.

    for (int i = (idx + 1); i < torrentsList_.GetItemCount(); i++)
    {
        DWORD_PTR data = torrentsList_.GetItemData(i);
        lt::sha1_hash* h = reinterpret_cast<lt::sha1_hash*>(data);

        torrents_[*h] -= 1;
    }

    torrentsList_.DeleteItem(idx);
    torrents_.erase(hash);
}

void MainFrame::UpdateTorrentUnsafe(lt::torrent_status& status)
{
    int idx = torrents_[status.info_hash];

    torrentsList_.SetItemText(idx, 0, Util::ToWideString(status.name).c_str());
    torrentsList_.SetItemText(idx, 1, std::to_wstring(status.queue_position).c_str());
    torrentsList_.SetItemText(idx, 2, Util::ToState(status).c_str());
    torrentsList_.SetItemText(idx, 3, Util::ToSpeed(status.download_payload_rate).c_str());
    torrentsList_.SetItemText(idx, 4, Util::ToSpeed(status.upload_payload_rate).c_str());
}

LRESULT MainFrame::OnContextMenu(CWindow cWindow, CPoint cPoint)
{
    if (cWindow != torrentsList_)
    {
        return FALSE;
    }

    torrentsList_.ScreenToClient(&cPoint);

    LVHITTESTINFO ht = { 0 };
    ht.pt = cPoint;
    torrentsList_.HitTest(&ht);

    if (!(ht.flags & LVHT_ONITEM))
    {
        return FALSE;
    }

    torrentsList_.ClientToScreen(&cPoint);
    DWORD_PTR data = torrentsList_.GetItemData(ht.iItem);
    lt::sha1_hash* hash = reinterpret_cast<lt::sha1_hash*>(data);
    lt::torrent_handle h = session_.find_torrent(*hash);

    if (!h.is_valid())
    {
        return FALSE;
    }

    lt::torrent_status status = h.status();

    HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(1300));
    hMenu = GetSubMenu(hMenu, 0);

    UINT checked = MF_CHECKED | MF_BYCOMMAND;
    UINT unchecked = MF_UNCHECKED | MF_BYCOMMAND;
    UINT disabled = MF_GRAYED | MF_BYCOMMAND;
    UINT enabled = MF_ENABLED | MF_BYCOMMAND;

    CMenu cm = (CMenu)hMenu;

    if (status.auto_managed)
    {
        cm.EnableMenuItem(ID_TORRENTCTX_PAUSE, disabled);
        cm.EnableMenuItem(ID_TORRENTCTX_RESUME, disabled);
    }
    else
    {
        cm.EnableMenuItem(ID_TORRENTCTX_PAUSE, status.paused ? disabled : enabled);
        cm.EnableMenuItem(ID_TORRENTCTX_RESUME, status.paused ? enabled : disabled);
    }

    cm.CheckMenuItem(ID_TORRENTCTX_MANAGED, status.auto_managed ? checked : unchecked);

    TrackPopupMenu(cm, TPM_LEFTALIGN | TPM_LEFTBUTTON, cPoint.x, cPoint.y, 0, m_hWnd, NULL);

    return FALSE;
}

LRESULT MainFrame::OnContextMenuCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    int idx = -1;
    while ((idx = torrentsList_.GetNextItem(idx, LVNI_SELECTED)) > -1)
    {
        DWORD_PTR data = torrentsList_.GetItemData(idx);
        lt::sha1_hash* hash = reinterpret_cast<lt::sha1_hash*>(data);
        lt::torrent_handle h = session_.find_torrent(*hash);

        if (!h.is_valid())
        {
            continue;
        }

        switch (wID)
        {
        case ID_TORRENTCTX_PAUSE:
            h.pause();
            break;

        case ID_TORRENTCTX_RESUME:
            h.resume();
            break;

        case ID_TORRENTCTX_MANAGED:
        {
            lt::torrent_status st = h.status();
            h.auto_managed(!st.auto_managed);
            break;
        }

        case ID_TORRENTCTX_RECHECK:
            h.force_recheck();
            break;

        case ID_TORRENTCTX_REMOVE:
            session_.remove_torrent(h);
            break;
        }
    }

    return FALSE;
}

LRESULT MainFrame::OnCopyData(HWND, PCOPYDATASTRUCT pCopyData)
{
    if (pCopyData->dwData == 1)
    {
        LPTSTR cmd = (LPTSTR)pCopyData->lpData;
        ParseCommandLine(cmd);
    }

    return 0;
}

LRESULT MainFrame::OnCreate(LPCREATESTRUCT)
{
    ui::Scaler scaler;

    ResizeClient(
        scaler.GetX(600),
        scaler.GetY(200));

    SetWindowText(L"PicoTorrent");

    m_hWndClient = torrentsList_.Create(m_hWnd,
        rcDefault,
        NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        WS_EX_CLIENTEDGE);

    torrentsList_.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

    torrentsList_.InsertColumn(0, _T("Name"), LVCFMT_LEFT, scaler.GetX(200), 0);
    torrentsList_.InsertColumn(1, _T("#"), LVCFMT_RIGHT, scaler.GetX(30), 1);
    torrentsList_.InsertColumn(2, _T("Status"), LVCFMT_LEFT, scaler.GetX(150), 2);
    torrentsList_.InsertColumn(3, _T("DL"), LVCFMT_RIGHT, scaler.GetX(80), 3);
    torrentsList_.InsertColumn(4, _T("UL"), LVCFMT_RIGHT, scaler.GetX(80), 4);

    SetTimer(99, 1000);

    ParseCommandLine(GetCommandLine());

    return 0;
}

LRESULT MainFrame::OnDestroy()
{
    KillTimer(99);
    return 0;
}

LRESULT MainFrame::OnTimer(UINT_PTR)
{
    session_.post_dht_stats();
    session_.post_session_stats();
    session_.post_torrent_updates();

    return FALSE;
}

LRESULT MainFrame::OnFileAddTorrent(UINT, int, CWindow)
{
    LPCTSTR filter =
        L"Torrent Files (*.torrent)\0*.torrent\0"
        L"All Files (*.*)\0*.*\0\0";

    CFileDialog dlg(TRUE,
        NULL,
        NULL,
        OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR,
        filter,
        m_hWnd);

    TCHAR buffer[65535] = { 0 };
    buffer[0] = _T('\0');

    dlg.m_ofn.lpstrFile = buffer;
    dlg.m_ofn.nMaxFile = _ARRAYSIZE(buffer);

    if (dlg.DoModal() == IDOK)
    {
        std::vector<std::wstring> files;

        if (dlg.m_ofn.nFileOffset < ::_tcslen(buffer))
        {
            files.push_back(buffer);
        }
        else
        {
            LPTSTR dir = buffer;
            LPTSTR file = buffer + dlg.m_ofn.nFileOffset;

            TCHAR fullPath[MAX_PATH] = { 0 };

            while (file != NULL && *file != _T('\0'))
            {
                fullPath[0] = _T('\0');
                PathCombine(fullPath, dir, file);

                files.push_back(fullPath);
                file = file + _tcslen(file) + 1;
            }
        }

        ShowAddTorrentDialog(files);
    }

    return FALSE;
}

LRESULT MainFrame::OnFileExit(UINT, int, CWindow)
{
    PostMessage(WM_CLOSE);
    return FALSE;
}

void MainFrame::ParseCommandLine(std::wstring cmdLine)
{
    if (cmdLine.empty())
    {
        return;
    }

    std::unique_ptr<CommandLine> cmd = CommandLine::Parse(cmdLine);
    std::vector<std::wstring> files = cmd->GetFiles();

    if (files.size() > 0)
    {
        ShowAddTorrentDialog(files);
    }
}

void MainFrame::ShowAddTorrentDialog(std::vector<std::wstring>& files)
{
    std::vector<lt::add_torrent_params> params;

    for (std::wstring& path : files)
    {
        lt::add_torrent_params p;
        p.save_path = Path::GetDefaultDownloadsPath().string();

        std::vector<char> buf;
        io::File::ReadBuffer(path, buf);

        lt::error_code ec;
        lt::bdecode_node node;
        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ec);

        if (ec)
        {
            BOOST_LOG_TRIVIAL(error)
                << "Could not decode " << path << ": "
                << ec.message() << ".";
            continue;
        }

        p.ti = boost::make_shared<lt::torrent_info>(node);
        params.push_back(p);
    }

    AddTorrentDialog* addTorrent = new AddTorrentDialog(
        std::make_shared<AddTorrentController>(session_, params));

    addTorrent->Create(m_hWnd);
    addTorrent->ShowWindow(SW_SHOW);

    SetForegroundWindow(addTorrent->m_hWnd);
}
