#pragma once

#include "../stdafx.h"
#include "../resources.h"

#include <memory>
#include <vector>

namespace libtorrent { class session; struct add_torrent_params; }
namespace UI { class ListView; }

namespace Dialogs
{
class AddTorrentDialog : public CDialogImpl<AddTorrentDialog>
{
public:
    enum { IDD = IDD_ADD_TORRENT };

    AddTorrentDialog(
        const std::shared_ptr<libtorrent::session>& session,
        const std::vector<std::shared_ptr<libtorrent::add_torrent_params>>& params);

private:
    void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnTorrentSelected(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnChangeStorageMode(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnChangeSavePath(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT OnLVGetItemText(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void ShowTorrent(int torrentIndex);

    BEGIN_MSG_MAP_EX(AddTorrentDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_HANDLER_EX(ID_TORRENT, CBN_SELENDOK, OnTorrentSelected)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
        COMMAND_ID_HANDLER_EX(ID_ADD_STORAGE_MODE_FULL, OnChangeStorageMode)
        COMMAND_ID_HANDLER_EX(ID_ADD_STORAGE_MODE_SPARSE, OnChangeStorageMode)
        COMMAND_ID_HANDLER_EX(ID_BROWSE, OnChangeSavePath)

        // ListView things
        MESSAGE_HANDLER_EX(PT_LV_GETITEMTEXT, OnLVGetItemText);
    END_MSG_MAP()

    std::shared_ptr<libtorrent::session> m_session;
    std::vector<std::shared_ptr<libtorrent::add_torrent_params>> m_params;
    std::shared_ptr<UI::ListView> m_fileList;
};
}
