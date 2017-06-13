#include "uTorrentSource.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/read_resume_data.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include <picotorrent/api.hpp>
#include <picotorrent/utils.hpp>

#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlcomcli.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <shellapi.h>
#include <ShlObj.h>
#include <Shlwapi.h>

#include "../resources.h"

namespace lt = libtorrent;
using Sources::uTorrentSource;

uTorrentSource::uTorrentSource(std::shared_ptr<IFileSystem> fileSystem)
    : m_fileSystem(fileSystem)
{
}

std::vector<Sources::Source::AddTorrentRequest> uTorrentSource::GetRequests()
{
    CEdit path = GetDlgItem(m_hWnd, ID_UT_RESUMEDAT);
    TCHAR p[MAX_PATH];
    path.GetWindowText(p, ARRAYSIZE(p));

    std::wstring dat_path = p;
    dat_path = dat_path.substr(0, dat_path.find_last_of('\\'));

    DirectoryPath data_path = dat_path;
    FileHandle dat = m_fileSystem->GetFile(p);

    if (!dat->Exists())
    {
        return std::vector<AddTorrentRequest>();
    }

    std::vector<AddTorrentRequest> result;

    std::error_code ec;
    std::vector<char> buf = dat->ReadAllBytes(ec);

    if (ec)
    {
        return result;
    }

    lt::bdecode_node node;
    lt::error_code ltec;
    lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

    if (ltec)
    {
        return result;
    }

    lt::entry e;
    e = node;

    lt::entry::dictionary_type d = e.dict();

    for (auto& p : d)
    {
        if (p.second.type() != lt::entry::data_type::dictionary_t)
        {
            continue;
        }

        lt::entry::dictionary_type item = p.second.dict();
        std::string path = item.at("path").string();
        path = path.substr(0, path.find_last_of('\\'));

        FilePath torrent_path = data_path + TWS(p.first);
        FileHandle torrent_file = m_fileSystem->GetFile(torrent_path);

        if (!torrent_file->Exists())
        {
            continue;
        }

        std::vector<char> tbuf = torrent_file->ReadAllBytes(ec);

        if (ec)
        {
            continue;
        }

        lt::bdecode_node tnode;
        lt::bdecode(&tbuf[0], &tbuf[0] + tbuf.size(), tnode, ltec);

        if (ltec)
        {
            continue;
        }

        AddTorrentRequest req;

        if (item.find("prio") != item.end())
        {
            std::string prio = item.at("prio").string();
            std::vector<char> prio_buf(prio.begin(), prio.end());

            for (auto& c : prio_buf)
            {
                if (c == 0x8)
                {
                    req.filePriorities.push_back(4);
                }
                else
                {
                    req.filePriorities.push_back(0);
                }
            }
        }

        req.savePath = path;
        req.ti = std::make_shared<lt::torrent_info>(tnode);

        result.push_back(req);
    }

    return result;
}

HWND uTorrentSource::GetWindowHandle(HINSTANCE hInstance, HWND hWndParent)
{
    return CreateDialogParam(
        hInstance,
        MAKEINTRESOURCE(IDD_CONFIG_UTORRENT),
        hWndParent,
        &uTorrentSource::DialogProc,
        reinterpret_cast<LPARAM>(this));
}

INT_PTR uTorrentSource::DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    uTorrentSource* qbs = reinterpret_cast<uTorrentSource*>(lParam);

    switch (uMsg)
    {
    case WM_COMMAND:
    {
        switch (HIWORD(wParam))
        {
        case BN_CLICKED:
        {
            switch (LOWORD(wParam))
            {
            case ID_UT_RESUMEDATBROWSE:
            {
                CComPtr<IFileOpenDialog> dlg;
                dlg.CoCreateInstance(CLSID_FileOpenDialog);

                //

                if (dlg->Show(hDlg) == S_OK)
                {
                    CComPtr<IShellItemArray> items = NULL;
                    dlg->GetResults(&items);

                    if (items == NULL)
                    {
                        break;
                    }

                    DWORD count;
                    items->GetCount(&count);

                    if (count > 0)
                    {
                        CComPtr<IShellItem> item = NULL;
                        items->GetItemAt(0, &item);

                        PWSTR path = NULL;
                        item->GetDisplayName(SIGDN_FILESYSPATH, &path);

                        SetDlgItemText(hDlg, ID_UT_RESUMEDAT, path);
                        CoTaskMemFree(path);
                    }
                }

                break;
            }
            }
        }
        }
        break;
    }
    case WM_INITDIALOG:
    {
        qbs->m_hWnd = hDlg;

        PWSTR buf;
        if (SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &buf) != S_OK)
        {
            return TRUE;
        }

        TCHAR qb[MAX_PATH];
        PathCombine(qb, buf, TEXT("uTorrent"));
        PathCombine(qb, qb, TEXT("resume.dat"));

        SetDlgItemText(hDlg, ID_UT_RESUMEDAT, qb);

        CoTaskMemFree(buf);

        return TRUE;
    }
    }

    return FALSE;
}
