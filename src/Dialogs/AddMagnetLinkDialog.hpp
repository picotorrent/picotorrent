#pragma once

#include <string>
#include <vector>

#include "../resources.h"
#include "../stdafx.h"

namespace libtorrent
{
    class torrent_info;
}

namespace Dialogs
{
class AddMagnetLinkDialog : public CDialogImpl<AddMagnetLinkDialog>
{
public:
    enum { IDD = IDD_ADD_MAGNET_LINK };

    AddMagnetLinkDialog(const std::vector<std::wstring>& magnetLinks);
    ~AddMagnetLinkDialog();

    std::vector<libtorrent::torrent_info> GetTorrentFiles();

private:
    std::vector<std::string> GetLinks();

    void OnAddMagnetLinks(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnDestroy();
    void OnEndDialog(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    LRESULT OnMetadataFound(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void UpdateCount();

    BEGIN_MSG_MAP_EX(AddMagnetLinkDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER_EX(ID_MAGNET_ADD_LINKS, OnAddMagnetLinks)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnEndDialog)
        MESSAGE_HANDLER_EX(PT_METADATAFOUND, OnMetadataFound)
    END_MSG_MAP()

    int m_waiting_for;
    std::vector<std::wstring> m_links;
    std::vector<libtorrent::torrent_info> m_torrent_files;
};
}
