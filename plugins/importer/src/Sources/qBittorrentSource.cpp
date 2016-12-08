#include "qBittorrentSource.hpp"

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
using Sources::qBittorrentSource;

qBittorrentSource::qBittorrentSource(std::shared_ptr<IFileSystem> fileSystem)
    : m_fileSystem(fileSystem)
{
}

std::vector<Sources::Source::AddTorrentRequest> qBittorrentSource::GetRequests()
{
    CEdit path = GetDlgItem(m_hWnd, ID_QB_APPLICATIONDATA);
    TCHAR p[MAX_PATH];
    path.GetWindowText(p, ARRAYSIZE(p));

    DirectoryHandle dir = m_fileSystem->GetDirectory(p);
    std::vector<AddTorrentRequest> result;

    if (!dir->Exists())
    {
        return std::vector<AddTorrentRequest>();
    }

    for (FileHandle file : dir->GetFiles(L"*.fastresume"))
    {
        FileHandle torrent = file->ChangeExtension(L".torrent");

        if (!file->Exists() || !torrent->Exists())
        {
            continue;
        }

        std::error_code ec;
        ByteBuffer buf = file->ReadAllBytes(ec);

        if (ec)
        {
            continue;
        }

        lt::error_code ltec;
        lt::add_torrent_params params = lt::read_resume_data(&buf[0], (int)buf.size(), ltec);

        if (ltec)
        {
            continue;
        }

        buf = torrent->ReadAllBytes(ec);

        if (ec)
        {
            continue;
        }

        params.ti = std::make_shared<lt::torrent_info>(&buf[0], (int)buf.size(), ltec);

        if (ltec)
        {
            continue;
        }

        AddTorrentRequest req;
        req.filePriorities = params.file_priorities;
        req.savePath = params.save_path;
        req.ti = params.ti;

        result.push_back(req);
    }

    return result;
}

HWND qBittorrentSource::GetWindowHandle(HINSTANCE hInstance, HWND hWndParent)
{
    return CreateDialogParam(
        hInstance,
        MAKEINTRESOURCE(IDD_CONFIG_QBITTORRENT),
        hWndParent,
        &qBittorrentSource::DialogProc,
        reinterpret_cast<LPARAM>(this));
}

INT_PTR qBittorrentSource::DialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    qBittorrentSource* qbs = reinterpret_cast<qBittorrentSource*>(lParam);

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
            case ID_QB_APPLICATIONDATABROWSE:
            {
                CComPtr<IFileOpenDialog> dlg;
                dlg.CoCreateInstance(CLSID_FileOpenDialog);

                dlg->SetOptions(FOS_PICKFOLDERS);

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

                        SetDlgItemText(hDlg, ID_QB_APPLICATIONDATA, path);
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
        if (SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &buf) != S_OK)
        {
            return TRUE;
        }

        TCHAR qb[MAX_PATH];
        PathCombine(qb, buf, TEXT("qBittorrent"));
        PathCombine(qb, qb, TEXT("BT_backup"));

        SetDlgItemText(hDlg, ID_QB_APPLICATIONDATA, qb);

        CoTaskMemFree(buf);

        return TRUE;
    }
    }

    return FALSE;
}
