#include "qBittorrentSource.hpp"

#include <libtorrent/bdecode.hpp>
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

std::vector<Sources::Source::PreviewItem> qBittorrentSource::GetPreview()
{
    CEdit path = GetDlgItem(m_hWnd, ID_QB_APPLICATIONDATA);
    TCHAR p[MAX_PATH];
    path.GetWindowText(p, ARRAYSIZE(p));

    DirectoryHandle dir = m_fileSystem->GetDirectory(p);
    std::vector<PreviewItem> result;

    if (!dir->Exists())
    {
        return std::vector<PreviewItem>();
    }

    for (FileHandle file : dir->GetFiles(L"*.torrent"))
    {
        // also get the .fastresume file
        FileHandle fastResume = file->ChangeExtension(L".fastresume");

        if (!fastResume->Exists())
        {
            continue;
        }

        std::error_code ec;
        ByteBuffer buf = file->ReadAllBytes(ec);

        if (ec)
        {
            continue;
        }

        lt::bdecode_node node;
        lt::error_code ltec;
        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

        if (ltec)
        {
            continue;
        }

        lt::torrent_info ti(node);

        // decode .fastresume file
        buf = fastResume->ReadAllBytes(ec);

        if (ec)
        {
            continue;
        }

        lt::bdecode(&buf[0], &buf[0] + buf.size(), node, ltec);

        if (ltec)
        {
            continue;
        }

        std::string savePath = node.dict_find_string_value("save_path").to_string();

        PreviewItem item{ TWS(ti.name()), TWS(savePath), ti.total_size() };
        result.push_back(item);
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

void qBittorrentSource::Import()
{

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
