#include "mainframe.h"

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session_handle.hpp>
#include <libtorrent/torrent_handle.hpp>
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
    std::wstring name = Util::ToWideString(status.name);

    int idx = torrentsList_.AddItem(torrentsList_.GetItemCount(), 0, name.c_str());
    torrentsList_.AddItem(idx, 1, std::to_wstring(status.queue_position + 1).c_str());
    torrentsList_.AddItem(idx, 2, Util::ToState(status.state).c_str());
    torrentsList_.AddItem(idx, 3, Util::ToSpeed(status.download_payload_rate).c_str());
    torrentsList_.AddItem(idx, 4, Util::ToSpeed(status.upload_payload_rate).c_str());

    torrents_[status.info_hash] = idx;
}

void MainFrame::UpdateTorrent(lt::torrent_status& status)
{
    int idx = torrents_[status.info_hash];

    torrentsList_.SetItemText(idx, 0, Util::ToWideString(status.name).c_str());
    torrentsList_.SetItemText(idx, 1, std::to_wstring(status.queue_position + 1).c_str());
    torrentsList_.SetItemText(idx, 2, Util::ToState(status.state).c_str());
    torrentsList_.SetItemText(idx, 3, Util::ToSpeed(status.download_payload_rate).c_str());
    torrentsList_.SetItemText(idx, 4, Util::ToSpeed(status.upload_payload_rate).c_str());
}

LRESULT MainFrame::OnCopyData(HWND hWnd, PCOPYDATASTRUCT pCopyData)
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

LRESULT MainFrame::OnTimer(UINT timerID)
{
    session_.post_dht_stats();
    session_.post_session_stats();
    session_.post_torrent_updates();

    return FALSE;
}

LRESULT MainFrame::OnFileAddTorrent(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    LPCTSTR files =
        L"Torrent Files (*.torrent)\0*.torrent\0"
        L"All Files (*.*)\0*.*\0\0";

    CFileDialog dlg(TRUE,
        NULL,
        NULL,
        OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR,
        files,
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

    return 0;
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
