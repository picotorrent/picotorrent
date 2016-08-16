#pragma once

#include "../stdafx.h"
#include "../resources.h"

#include <memory>
#include <vector>

namespace libtorrent { struct add_torrent_params; }
namespace UI { class ListView; }

namespace Dialogs
{
class AddTorrentDialog : public CDialogImpl<AddTorrentDialog>
{
public:
    enum { IDD = IDD_ADD_TORRENT };

    AddTorrentDialog(
        const std::vector<std::shared_ptr<libtorrent::add_torrent_params>>& params);

    std::vector<std::shared_ptr<libtorrent::add_torrent_params>>& GetParams();

private:
    std::wstring GetPriorityString(int priority);

    void OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnTorrentSelected(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnChangeStorageMode(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnChangeSavePath(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT OnLVGetItemText(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnLVShowContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void ShowTorrent(int torrentIndex);

    BEGIN_MSG_MAP_EX(AddTorrentDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER_EX(ID_TORRENT, CBN_SELENDOK, OnTorrentSelected)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnEndDialog)
        COMMAND_ID_HANDLER_EX(IDOK, OnEndDialog)
        COMMAND_ID_HANDLER_EX(ID_ADD_STORAGE_MODE_FULL, OnChangeStorageMode)
        COMMAND_ID_HANDLER_EX(ID_ADD_STORAGE_MODE_SPARSE, OnChangeStorageMode)
        COMMAND_ID_HANDLER_EX(ID_BROWSE, OnChangeSavePath)

        // ListView things
        MESSAGE_HANDLER_EX(PT_LV_GETITEMTEXT, OnLVGetItemText)
        MESSAGE_HANDLER_EX(PT_LV_SHOWCONTEXTMENU, OnLVShowContextMenu)
    END_MSG_MAP()

    std::vector<std::shared_ptr<libtorrent::add_torrent_params>> m_params;
    std::shared_ptr<UI::ListView> m_fileList;

    CComboBox m_torrents;
    CEdit m_size;
    CEdit m_savePath;
    CButton m_storageFull;
    CButton m_storageSparse;
};
}
