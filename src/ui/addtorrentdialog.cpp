#include "addtorrentdialog.h"

#include "renamefiledialog.h"
#include "../controllers/addtorrentcontroller.h"
#include "../util.h"

using namespace pico;

AddTorrentDialog::AddTorrentDialog(std::shared_ptr<AddTorrentController> controller)
    : controller_(controller)
{
}

AddTorrentDialog::~AddTorrentDialog()
{
}

LRESULT AddTorrentDialog::OnInit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    SetWindowText(L"Add torrent(s)");
    DlgResize_Init();

    // Set up list view
    CListViewCtrl listView = (CListViewCtrl)GetDlgItem(ID_ADDTORRENT_FILES);
    listView.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 200, 0);
    listView.InsertColumn(1, _T("Size"), LVCFMT_RIGHT, 80, 1);
    listView.InsertColumn(2, _T("Priority"), LVCFMT_LEFT, 80, 2);

    // Torrents combo box
    CComboBox torrents = (CComboBox)GetDlgItem(ID_ADDTORRENT_TORRENTS);

    for (uint64_t i = 0; i < controller_->GetCount(); i++)
    {
        torrents.AddString(controller_->GetName(i).c_str());
    }

    torrents.SetCurSel(0);
    ShowTorrent(0);

    return TRUE;
}

LRESULT AddTorrentDialog::OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (wID == IDOK)
    {
        controller_->AddTorrents();
    }

    DestroyWindow();
    return TRUE;
}

void AddTorrentDialog::OnFinalMessage(HWND hWnd)
{
    delete this;
}

LRESULT AddTorrentDialog::OnTorrentSelected(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CComboBox torrents = (CComboBox)GetDlgItem(ID_ADDTORRENT_TORRENTS);
    ShowTorrent(torrents.GetCurSel());

    return FALSE;
}

void AddTorrentDialog::ShowTorrent(uint64_t index)
{
    CStatic size = (CStatic)GetDlgItem(ID_ADDTORRENT_SIZE);
    CStatic comment = (CStatic)GetDlgItem(ID_ADDTORRENT_COMMENT);
    CStatic creationDate = (CStatic)GetDlgItem(ID_ADDTORRENT_CREATIONDATE);
    CStatic creator = (CStatic)GetDlgItem(ID_ADDTORRENT_CREATOR);

    size.SetWindowTextW(controller_->GetSize(index).c_str());
    comment.SetWindowTextW(controller_->GetComment(index).c_str());
    creationDate.SetWindowTextW(controller_->GetCreationDate(index).c_str());
    creator.SetWindowTextW(controller_->GetCreator(index).c_str());

    CEdit savePath = (CEdit)GetDlgItem(ID_ADDTORRENT_SAVEPATH);
    savePath.SetWindowTextW(controller_->GetSavePath(index).c_str());

    CListViewCtrl files = (CListViewCtrl)GetDlgItem(ID_ADDTORRENT_FILES);
    files.DeleteAllItems();
    files.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

    for (uint64_t i = 0; i < controller_->GetFileCount(index); i++)
    {
        std::wstring fileName = controller_->GetFileName(index, i);
        std::wstring fileSize = controller_->GetFileSize(index, i);
        std::wstring filePrio = Util::ToPriority(controller_->GetFilePriority(index, i));

        int idx = files.AddItem(files.GetItemCount(), 0, fileName.c_str());
        files.AddItem(idx, 1, fileSize.c_str());
        files.AddItem(idx, 2, filePrio.c_str());
    }
}

LRESULT AddTorrentDialog::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    CListViewCtrl files = (CListViewCtrl)GetDlgItem(ID_ADDTORRENT_FILES);
    
    if ((HWND)wParam != files.m_hWnd)
    {
        bHandled = FALSE;
        return FALSE;
    }

    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);

    if (x == -1 && y == -1)
    {
        bHandled = FALSE;
        return FALSE;
    }

    CComboBox torrents = (CComboBox)GetDlgItem(ID_ADDTORRENT_TORRENTS);
    int torrentIndex = torrents.GetCurSel();

    // TODO(multi file selection)

    int fileIndex = files.GetSelectedIndex();

    int prio = controller_->GetFilePriority(torrentIndex, fileIndex);

    HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(1778)); // TODO(does not work with #define'd id)
    hMenu = GetSubMenu(hMenu, 0);

    UINT checked = MF_CHECKED | MF_BYCOMMAND;
    UINT unchecked = MF_UNCHECKED | MF_BYCOMMAND;

    CMenu cm = (CMenu)hMenu;
    cm.CheckMenuItem(ID_ADDTORRENT_PRIO_NORMAL, prio == 1 ? checked : unchecked);
    cm.CheckMenuItem(ID_ADDTORRENT_PRIO_HIGH, prio == 6 ? checked : unchecked);
    cm.CheckMenuItem(ID_ADDTORRENT_PRIO_MAX, prio == 7 ? checked : unchecked);
    cm.CheckMenuItem(ID_ADDTORRENT_PRIO_DND, prio == 0 ? checked : unchecked);

    TrackPopupMenu(cm.m_hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, x, y, 0, m_hWnd, NULL);

    return FALSE;
}

LRESULT AddTorrentDialog::OnRenameFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CComboBox torrents = (CComboBox)GetDlgItem(ID_ADDTORRENT_TORRENTS);
    CListViewCtrl files = (CListViewCtrl)GetDlgItem(ID_ADDTORRENT_FILES);

    int torrentIndex = torrents.GetCurSel();
    int fileIndex = files.GetSelectedIndex();

    std::wstring name = controller_->GetFileName(torrentIndex, fileIndex);

    RenameFileDialog dlg(name);
    if (dlg.DoModal() != IDOK)
    {
        return FALSE;
    }

    name = dlg.GetValue();

    controller_->SetFileName(torrentIndex, fileIndex, name);
    files.SetItemText(fileIndex, 0, name.c_str());
    
    return FALSE;
}

LRESULT AddTorrentDialog::OnPrioritizeFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CComboBox torrents = (CComboBox)GetDlgItem(ID_ADDTORRENT_TORRENTS);
    CListViewCtrl files = (CListViewCtrl)GetDlgItem(ID_ADDTORRENT_FILES);

    int torrentIndex = torrents.GetCurSel();
    int fileIndex = files.GetSelectedIndex();

    switch (wID)
    {
    case ID_ADDTORRENT_PRIO_MAX:
        controller_->SetFilePriority(torrentIndex, fileIndex, 7);
        break;

    case ID_ADDTORRENT_PRIO_HIGH:
        controller_->SetFilePriority(torrentIndex, fileIndex, 6);
        break;

    case ID_ADDTORRENT_PRIO_NORMAL:
        controller_->SetFilePriority(torrentIndex, fileIndex, 1);
        break;

    case ID_ADDTORRENT_PRIO_DND:
        controller_->SetFilePriority(torrentIndex, fileIndex, 0);
        break;
    }

    int prio = controller_->GetFilePriority(torrentIndex, fileIndex);
    files.SetItemText(fileIndex, 2, Util::ToPriority(prio).c_str());

    return FALSE;
}

LRESULT AddTorrentDialog::BrowseSavePath(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CFolderDialog dlg;

    if (dlg.DoModal() != IDOK)
    {
        return FALSE;
    }

    CComboBox torrents = (CComboBox)GetDlgItem(ID_ADDTORRENT_TORRENTS);
    CEdit savePath = (CEdit)GetDlgItem(ID_ADDTORRENT_SAVEPATH);

    savePath.SetWindowTextW(dlg.GetFolderPath());
    controller_->SetSavePath(torrents.GetCurSel(), dlg.GetFolderPath());

    return TRUE;
}
