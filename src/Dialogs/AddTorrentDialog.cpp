#include "AddTorrentDialog.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include <strsafe.h>

#include "../Dialogs/OpenFileDialog.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"
#include "../UI/ListView.hpp"

#define LV_COL_NAME 0
#define LV_COL_SIZE 1
#define LV_COL_PRIO 2

const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

namespace lt = libtorrent;
using Dialogs::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(
    const std::vector<std::shared_ptr<lt::add_torrent_params>>& params)
    : m_params(params)
{
}

std::vector<std::shared_ptr<libtorrent::add_torrent_params>>& AddTorrentDialog::GetParams()
{
    return m_params;
}

std::wstring AddTorrentDialog::GetPriorityString(int priority)
{
    switch (priority)
    {
    case PRIORITY_DO_NOT_DOWNLOAD:
        return TRW("do_not_download");
    case PRIORITY_NORMAL:
        return TRW("normal");
    case PRIORITY_HIGH:
        return TRW("high");
    case PRIORITY_MAXIMUM:
        return TRW("maximum");
    default:
        return TRW("unknown");
    }
}

void AddTorrentDialog::OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    EndDialog(nID);
}

BOOL AddTorrentDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    CenterWindow();

    // Hook up controls
    m_torrents = GetDlgItem(ID_TORRENT);
    m_size = GetDlgItem(ID_SIZE);
    m_savePath = GetDlgItem(ID_SAVE_PATH);
    m_storageFull = GetDlgItem(ID_ADD_STORAGE_MODE_FULL);
    m_storageSparse = GetDlgItem(ID_ADD_STORAGE_MODE_SPARSE);

    // Localize
    SetWindowText(TRW("add_torrent_s"));
    SetDlgItemText(ID_TORRENT_TEXT, TRW("torrent"));
    SetDlgItemText(ID_SIZE_TEXT, TRW("size"));
    SetDlgItemText(ID_SAVE_PATH_TEXT, TRW("save_path"));
    SetDlgItemText(ID_BROWSE, TRW("browse"));
    SetDlgItemText(ID_STORAGE_GROUP, TRW("storage"));
    SetDlgItemText(IDOK, TRW("add_torrent_s"));
    SetDlgItemText(ID_ADD_STORAGE_MODE_TEXT, TRW("storage_mode"));
    SetDlgItemText(ID_ADD_STORAGE_MODE_SPARSE, TRW("sparse"));
    SetDlgItemText(ID_ADD_STORAGE_MODE_FULL, TRW("full"));

    // Set up list view
    m_fileList = std::make_shared<UI::ListView>(GetDlgItem(ID_FILES));
    m_fileList->AddColumn(LV_COL_NAME, TRW("name"), SX(270), UI::ListView::ColumnType::Text);
    m_fileList->AddColumn(LV_COL_SIZE, TRW("size"), SX(80), UI::ListView::ColumnType::Number);
    m_fileList->AddColumn(LV_COL_PRIO, TRW("priority"), SX(120), UI::ListView::ColumnType::Text);

    for (auto p : m_params)
    {
        m_torrents.AddString(ToWideString(p->ti->name()).c_str());
    }

    m_torrents.SetCurSel(0);
    ShowTorrent(0);

    return 0;
}

void AddTorrentDialog::OnChangeStorageMode(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());

    switch (nID)
    {
    case ID_ADD_STORAGE_MODE_FULL:
        m_storageFull.SetCheck(BST_CHECKED);
        m_storageSparse.SetCheck(BST_UNCHECKED);
        prm->storage_mode = lt::storage_mode_t::storage_mode_allocate;
        break;
    case ID_ADD_STORAGE_MODE_SPARSE:
        m_storageSparse.SetCheck(BST_CHECKED);
        m_storageFull.SetCheck(BST_UNCHECKED);
        prm->storage_mode = lt::storage_mode_t::storage_mode_sparse;
        break;
    }
}

void AddTorrentDialog::OnChangeSavePath(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    Dialogs::OpenFileDialog openDialog;
    openDialog.SetGuid(DLG_SAVE);
    openDialog.SetOptions(openDialog.GetOptions() | FOS_PICKFOLDERS);
    openDialog.Show();

    auto res = openDialog.GetPaths();
    if (res.empty()) { return; }

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());
    prm->save_path = ToString(res[0]);
    m_savePath.SetWindowText(res[0].c_str());
}

LRESULT AddTorrentDialog::OnLVGetItemText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());

    UI::ListView::GetItemText* git = reinterpret_cast<UI::ListView::GetItemText*>(lParam);

    switch (git->column_id)
    {
    case LV_COL_NAME:
        git->text = ToWideString(prm->ti->files().file_path(git->item_index));
        break;
    case LV_COL_SIZE:
        TCHAR s[1024];
        StrFormatByteSize64(prm->ti->files().file_size(git->item_index), s, ARRAYSIZE(s));
        git->text = s;
        break;
    case LV_COL_PRIO:
        if (prm->file_priorities.size() > git->item_index)
        {
            git->text = GetPriorityString(prm->file_priorities[git->item_index]);
        }
        else
        {
            git->text = GetPriorityString(PRIORITY_NORMAL);
        }
        break;
    }

    return 0;
}

LRESULT AddTorrentDialog::OnLVShowContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::ListView::ShowContextMenu* scm = reinterpret_cast<UI::ListView::ShowContextMenu*>(lParam);
    if (scm->selected_indices.empty()) { return 0; }
    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());

    HMENU prioMenu = CreateMenu();
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_MAX, TRW("maximum"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_HIGH, TRW("high"));
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_NORMAL, TRW("normal"));
    AppendMenu(prioMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(prioMenu, MF_STRING, TORRENT_FILE_PRIO_SKIP, TRW("do_not_download"));

    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_POPUP, (UINT_PTR)prioMenu, TRW("priority"));

    // If only one file is selected, check that files priority
    if (scm->selected_indices.size() == 1)
    {
        int idx = scm->selected_indices[0];
        int prio = prm->file_priorities.size() > idx
            ? prm->file_priorities[idx]
            : PRIORITY_NORMAL;

        switch (prio)
        {
        case PRIORITY_DO_NOT_DOWNLOAD:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_SKIP, MF_BYCOMMAND | MF_CHECKED);
            break;
        case PRIORITY_NORMAL:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_NORMAL, MF_BYCOMMAND | MF_CHECKED);
            break;
        case PRIORITY_HIGH:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_HIGH, MF_BYCOMMAND | MF_CHECKED);
            break;
        case PRIORITY_MAXIMUM:
            CheckMenuItem(prioMenu, TORRENT_FILE_PRIO_MAX, MF_BYCOMMAND | MF_CHECKED);
            break;
        }
    }

    int res = TrackPopupMenu(
        menu,
        TPM_NONOTIFY | TPM_RETURNCMD,
        scm->point.x,
        scm->point.y,
        0,
        m_fileList->GetHandle(),
        NULL);

    for (int idx : scm->selected_indices)
    {
        if (prm->file_priorities.size() < idx + 1)
        {
            prm->file_priorities.resize(idx + 1, PRIORITY_NORMAL);
        }

        switch (res)
        {
        case TORRENT_FILE_PRIO_SKIP:
            prm->file_priorities[idx] = PRIORITY_DO_NOT_DOWNLOAD;
            break;
        case TORRENT_FILE_PRIO_NORMAL:
            prm->file_priorities[idx] = PRIORITY_NORMAL;
            break;
        case TORRENT_FILE_PRIO_HIGH:
            prm->file_priorities[idx] = PRIORITY_HIGH;
            break;
        case TORRENT_FILE_PRIO_MAX:
            prm->file_priorities[idx] = PRIORITY_MAXIMUM;
            break;
        }
    }

    auto idxLo = std::min_element(scm->selected_indices.begin(), scm->selected_indices.end());
    auto idxHi = std::max_element(scm->selected_indices.begin(), scm->selected_indices.end());

    m_fileList->RedrawItems(*idxLo, *idxHi);

    return 0;
}

void AddTorrentDialog::OnTorrentSelected(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    ShowTorrent(m_torrents.GetCurSel());
}

void AddTorrentDialog::ShowTorrent(int torrentIndex)
{
    if (torrentIndex >= m_params.size())
    {
        // LOG
        return;
    }

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(torrentIndex);
    boost::shared_ptr<lt::torrent_info> ti = prm->ti;

    m_savePath.SetWindowText(ToWideString(prm->save_path).c_str());
    m_storageFull.SetCheck(BST_UNCHECKED);
    m_storageSparse.SetCheck(BST_UNCHECKED);

    switch (prm->storage_mode)
    {
    case lt::storage_mode_t::storage_mode_allocate:
        m_storageFull.SetCheck(BST_CHECKED);
        break;
    case lt::storage_mode_t::storage_mode_sparse:
        m_storageSparse.SetCheck(BST_CHECKED);
        break;
    }

    if (ti)
    {
        TCHAR s[1024];
        StrFormatByteSize64(ti->total_size(), s, ARRAYSIZE(s));

        m_size.SetWindowText(s);
        m_fileList->SetItemCount(ti->num_files());
    }
    else
    {
        m_fileList->SetItemCount(0);
    }
}
