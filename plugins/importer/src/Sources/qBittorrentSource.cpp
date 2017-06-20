#include "qBittorrentSource.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>

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

namespace fs = std::experimental::filesystem::v1;
namespace lt = libtorrent;
using Sources::qBittorrentSource;

std::vector<Sources::Source::AddTorrentRequest> qBittorrentSource::GetRequests()
{
    CEdit path = GetDlgItem(m_hWnd, ID_QB_APPLICATIONDATA);
    TCHAR p[MAX_PATH];
    path.GetWindowText(p, ARRAYSIZE(p));

    if (!fs::exists(p))
    {
        return std::vector<AddTorrentRequest>();
    }

    std::vector<AddTorrentRequest> result;

    for (auto& entry : fs::directory_iterator(p))
    {
        fs::path path = entry.path();

        if (path.extension() != ".fastresume")
        {
            continue;
        }

        fs::path torrent = path.replace_extension(".torrent");

        if (!fs::exists(path) || !fs::exists(torrent))
        {
            continue;
        }

        std::ifstream resume_input(path, std::ios::binary);
        std::stringstream rss;
        rss << resume_input.rdbuf();
        std::string buf = rss.str();

        lt::error_code ltec;
        lt::add_torrent_params params = lt::read_resume_data(
            &buf[0],
            static_cast<int>(buf.size()),
            ltec);

        if (ltec)
        {
            continue;
        }

        std::ifstream torrent_input(torrent, std::ios::binary);
        std::stringstream tss;
        tss << torrent_input.rdbuf();
        buf = tss.str();

        params.ti = std::make_shared<lt::torrent_info>(
            &buf[0],
            static_cast<int>(buf.size()),
            ltec);

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
