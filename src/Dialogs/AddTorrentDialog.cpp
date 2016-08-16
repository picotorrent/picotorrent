#include "AddTorrentDialog.hpp"

#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>

#include <strsafe.h>

#include "../Dialogs/OpenFileDialog.hpp"
#include "../UI/ListView.hpp"
#include "../Translator.hpp"

#define LV_COL_NAME 0
#define LV_COL_SIZE 1
#define LV_COL_PRIO 2

const GUID DLG_SAVE = { 0x7D5FE367, 0xE148, 0x4A96,{ 0xB3, 0x26, 0x42, 0xEF, 0x23, 0x7A, 0x36, 0x61 } };

namespace lt = libtorrent;
using Dialogs::AddTorrentDialog;

AddTorrentDialog::AddTorrentDialog(
    const std::shared_ptr<lt::session>& session,
    const std::vector<std::shared_ptr<lt::add_torrent_params>>& params)
    : m_session(session),
    m_params(params)
{
}

void AddTorrentDialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    EndDialog(nID);
}

BOOL AddTorrentDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    CenterWindow();

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
    m_fileList->AddColumn(LV_COL_NAME, TRW("name"), 270, UI::ListView::ColumnType::Text);
    m_fileList->AddColumn(LV_COL_SIZE, TRW("size"), 80, UI::ListView::ColumnType::Number);
    m_fileList->AddColumn(LV_COL_PRIO, TRW("priority"), 120, UI::ListView::ColumnType::Text);

    // Set up torrents
    CComboBox cmb(GetDlgItem(ID_TORRENT));

    for (auto p : m_params)
    {
        cmb.AddString(ToWideString(p->ti->name()).c_str());
    }

    cmb.SetCurSel(0);
    ShowTorrent(0);

    return 0;
}

void AddTorrentDialog::OnChangeStorageMode(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CComboBox cmb = GetDlgItem(ID_TORRENT);
    CButton storeFull = GetDlgItem(ID_ADD_STORAGE_MODE_FULL);
    CButton storeSparse = GetDlgItem(ID_ADD_STORAGE_MODE_SPARSE);

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(cmb.GetCurSel());

    switch (nID)
    {
    case ID_ADD_STORAGE_MODE_FULL:
        storeFull.SetCheck(BST_CHECKED);
        storeSparse.SetCheck(BST_UNCHECKED);
        prm->storage_mode = lt::storage_mode_t::storage_mode_allocate;
        break;
    case ID_ADD_STORAGE_MODE_SPARSE:
        storeSparse.SetCheck(BST_CHECKED);
        storeFull.SetCheck(BST_UNCHECKED);
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

    CComboBox cmb = GetDlgItem(ID_TORRENT);
    CEdit savePath = GetDlgItem(ID_SAVE_PATH);

    std::shared_ptr<lt::add_torrent_params> prm = m_params.at(cmb.GetCurSel());
    prm->save_path = ToString(res[0]);
    savePath.SetWindowText(res[0].c_str());
}

LRESULT AddTorrentDialog::OnLVGetItemText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::ListView::GetItemText* git = reinterpret_cast<UI::ListView::GetItemText*>(lParam);
    git->text = TEXT("Hello!");

    return 0;
}

void AddTorrentDialog::OnTorrentSelected(UINT uNotifyCode, int nID, CWindow wndCtl)
{
    CComboBox cmb(GetDlgItem(ID_TORRENT));
    ShowTorrent(cmb.GetCurSel());
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

    CEdit size = GetDlgItem(ID_SIZE);
    CEdit savePath = GetDlgItem(ID_SAVE_PATH);
    CButton storeFull = GetDlgItem(ID_ADD_STORAGE_MODE_FULL);
    CButton storeSparse = GetDlgItem(ID_ADD_STORAGE_MODE_SPARSE);

    savePath.SetWindowText(ToWideString(prm->save_path).c_str());
    storeFull.SetCheck(BST_UNCHECKED);
    storeSparse.SetCheck(BST_UNCHECKED);

    switch (prm->storage_mode)
    {
    case lt::storage_mode_t::storage_mode_allocate:
        storeFull.SetCheck(BST_CHECKED);
        break;
    case lt::storage_mode_t::storage_mode_sparse:
        storeSparse.SetCheck(BST_CHECKED);
        break;
    }

    if (ti)
    {
        TCHAR s[1024];
        StrFormatByteSize64(ti->total_size(), s, ARRAYSIZE(s));
        size.SetWindowText(s);

        m_fileList->SetItemCount(ti->num_files());
    }
    else
    {
        m_fileList->SetItemCount(0);
    }
}
