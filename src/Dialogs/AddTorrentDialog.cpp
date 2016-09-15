#include "AddTorrentDialog.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include <strsafe.h>

#include "../Commands/PrioritizeFilesCommand.hpp"
#include "../Dialogs/OpenFileDialog.hpp"
#include "../Models/TorrentFile.hpp"
#include "../resources.h"
#include "../Scaler.hpp"
#include "../Translator.hpp"
#include "../UI/TorrentFileListView.hpp"

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

    m_fileList = std::make_shared<UI::TorrentFileListView>(GetDlgItem(ID_FILES));

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

LRESULT AddTorrentDialog::OnPrioritizeFiles(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto cmd = reinterpret_cast<Commands::PrioritizeFilesCommand*>(lParam);
    int current = m_torrents.GetCurSel();
    auto prm = m_params.at(current);
    auto files = prm->ti->files();

    for (int idx : cmd->indices)
    {
        if (prm->file_priorities.size() <= (size_t)idx)
        {
            prm->file_priorities.resize(idx + 1, PRIORITY_NORMAL);
        }

        prm->file_priorities.at(idx) = cmd->priority;

        // Update model
        Models::TorrentFile tf{ idx };
        tf.name = TWS(files.file_path(idx));
        tf.priority = cmd->priority;
        tf.size = files.file_size(idx);

        m_fileList->Update(tf);
    }

    return FALSE;
}

void AddTorrentDialog::OnSavePathChanged(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(m_torrents.GetCurSel());
    prm->save_path = m_savePath.GetValueA();
}

void AddTorrentDialog::OnTorrentSelected(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    ShowTorrent(m_torrents.GetCurSel());
}

void AddTorrentDialog::ShowTorrent(size_t torrentIndex)
{
    if (torrentIndex >= m_params.size())
    {
        // LOG
        return;
    }

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(torrentIndex);
    boost::shared_ptr<lt::torrent_info> ti = prm->ti;

    m_fileList->RemoveAll();
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

        // Add model to list
        const lt::file_storage& files = ti->files();

        for (int i = 0; i < files.num_files(); i++)
        {
            Models::TorrentFile tf{ i };
            tf.name = TWS(files.file_path(i));
            tf.priority = prm->file_priorities.size() > (size_t)i
                ? prm->file_priorities.at(i)
                : PRIORITY_NORMAL;
            tf.size = files.file_size(i);

            m_fileList->Add(tf);
        }

        m_fileList->SetItemCount(ti->num_files());
    }
    else
    {
        m_fileList->SetItemCount(0);
    }
}
