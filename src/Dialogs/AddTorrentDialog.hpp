#pragma once

#include "../stdafx.h"
#include "../resources.h"
#include "../UI/TextBox.hpp"

#include <memory>
#include <vector>

namespace libtorrent { struct add_torrent_params; }
namespace UI { class TorrentFileListView; }

namespace Dialogs
{
class AddTorrentDialog : public CDialogImpl<AddTorrentDialog>, public CDialogResize<AddTorrentDialog>
{
public:
    enum { IDD = IDD_ADD_TORRENT };

    AddTorrentDialog(
        const std::vector<std::shared_ptr<libtorrent::add_torrent_params>>& params);

    std::vector<std::shared_ptr<libtorrent::add_torrent_params>>& GetParams();

private:
    void OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnTorrentSelected(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnSavePathChanged(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnChangeStorageMode(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnChangeSavePath(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT OnPrioritizeFiles(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void OnShowFileFilter(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT OnShowFileFilterNotify(LPNMHDR lpnmHdr);
    void ShowTorrent(size_t torrentIndex);
    void FilterFiles(bool include);

    BEGIN_MSG_MAP_EX(AddTorrentDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
        MESSAGE_HANDLER_EX(PT_PRIORITIZEFILES, OnPrioritizeFiles)
        COMMAND_HANDLER_EX(ID_TORRENT, CBN_SELENDOK, OnTorrentSelected)
        COMMAND_HANDLER_EX(ID_SAVE_PATH, EN_CHANGE, OnSavePathChanged)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnEndDialog)
        COMMAND_ID_HANDLER_EX(IDOK, OnEndDialog)
        COMMAND_ID_HANDLER_EX(ID_ADD_STORAGE_MODE_FULL, OnChangeStorageMode)
        COMMAND_ID_HANDLER_EX(ID_ADD_STORAGE_MODE_SPARSE, OnChangeStorageMode)
        COMMAND_ID_HANDLER_EX(ID_BROWSE, OnChangeSavePath)
        COMMAND_ID_HANDLER_EX(ID_INCLUDE_FILE_FILTER, OnShowFileFilter)
        COMMAND_ID_HANDLER_EX(ID_EXCLUDE_FILE_FILTER, OnShowFileFilter)
        NOTIFY_HANDLER_EX(ID_INCLUDE_FILE_FILTER, BCN_DROPDOWN, OnShowFileFilterNotify)
        NOTIFY_HANDLER_EX(ID_EXCLUDE_FILE_FILTER, BCN_DROPDOWN, OnShowFileFilterNotify)
        CHAIN_MSG_MAP(CDialogResize<AddTorrentDialog>)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(AddTorrentDialog)
        DLGRESIZE_CONTROL(ID_TORRENT, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(ID_STORAGE_GROUP, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(ID_SAVE_PATH, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(ID_BROWSE, DLSZ_MOVE_X)
        DLGRESIZE_CONTROL(ID_FILES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(ID_EXCLUDE_FILE_FILTER, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(ID_INCLUDE_FILE_FILTER, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    std::vector<std::shared_ptr<libtorrent::add_torrent_params>> m_params;
    std::shared_ptr<UI::TorrentFileListView> m_fileList;

    CComboBox m_torrents;
    CEdit m_size;
    UI::TextBox m_savePath;
    CButton m_storageFull;
    CButton m_storageSparse;
    CButton m_excludeFilter;
    CButton m_includeFilter;
};
}
