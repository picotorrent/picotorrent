#pragma once

#include <memory>
#include "../stdafx.h"

namespace pico
{
    class AddTorrentController;

    class AddTorrentDialog : public CDialogImpl<AddTorrentDialog>, public CDialogResize<AddTorrentDialog>
    {
    public:
        enum { IDD = IDD_ADDTORRENT };

        AddTorrentDialog(std::shared_ptr<AddTorrentController> controller);
        ~AddTorrentDialog();

        LRESULT OnInit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
        LRESULT OnClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnRenameFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnPrioritizeFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        void OnFinalMessage(HWND hWnd);
        LRESULT OnTorrentSelected(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
        LRESULT OnContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
        LRESULT BrowseSavePath(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    private:
        BEGIN_MSG_MAP(AddTorrentDialog)
            MESSAGE_HANDLER(WM_INITDIALOG, OnInit)
            COMMAND_ID_HANDLER(ID_ADDTORRENT_BROWSE, BrowseSavePath)
            COMMAND_ID_HANDLER(IDCANCEL, OnClose)
            COMMAND_ID_HANDLER(IDOK, OnClose)
            COMMAND_ID_HANDLER(ID_ADDTORRENT_RENAME_FILE, OnRenameFile)
            COMMAND_ID_HANDLER(ID_ADDTORRENT_PRIO_MAX, OnPrioritizeFile)
            COMMAND_ID_HANDLER(ID_ADDTORRENT_PRIO_HIGH, OnPrioritizeFile)
            COMMAND_ID_HANDLER(ID_ADDTORRENT_PRIO_NORMAL, OnPrioritizeFile)
            COMMAND_ID_HANDLER(ID_ADDTORRENT_PRIO_DND, OnPrioritizeFile)
            COMMAND_HANDLER(ID_ADDTORRENT_TORRENTS, CBN_SELENDOK, OnTorrentSelected)
            MESSAGE_HANDLER(WM_CONTEXTMENU, OnContextMenu)
            CHAIN_MSG_MAP(CDialogResize<AddTorrentDialog>)
        END_MSG_MAP()

        BEGIN_DLGRESIZE_MAP(AddTorrentDialog)
            DLGRESIZE_CONTROL(ID_ADDTORRENT_GROUP_TORRENTS, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_ADDTORRENT_TORRENTS, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_ADDTORRENT_GROUP_STORAGE, DLSZ_SIZE_X | DLSZ_SIZE_Y)
            DLGRESIZE_CONTROL(ID_ADDTORRENT_SAVEPATH, DLSZ_SIZE_X)
            DLGRESIZE_CONTROL(ID_ADDTORRENT_BROWSE, DLSZ_MOVE_X)
            DLGRESIZE_CONTROL(ID_ADDTORRENT_FILES, DLSZ_SIZE_X | DLSZ_SIZE_Y)
            DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
            DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
        END_DLGRESIZE_MAP()

        void ShowTorrent(uint64_t index);

        std::shared_ptr<AddTorrentController> controller_;
    };
}
