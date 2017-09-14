#pragma once

#include <string>
#include <vector>

#include "../resources.h"
#include "../stdafx.h"

namespace libtorrent
{
	struct add_torrent_params;
}

namespace Dialogs
{
class AddMagnetLinkDialog : public CDialogImpl<AddMagnetLinkDialog>
{
public:
    enum { IDD = IDD_ADD_MAGNET_LINK };

    AddMagnetLinkDialog(const std::vector<std::wstring>& magnetLinks);
    ~AddMagnetLinkDialog();

	std::vector<libtorrent::add_torrent_params> GetTorrentParams();

private:
    bool IsInfoHash(std::string const& link);
    bool IsMagnetLink(std::string const& link);
	std::vector<libtorrent::add_torrent_params> ParseTorrentParams();

    void OnAddMagnetLinks(UINT uNotifyCode, int nID, CWindow wndCtl);
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

    BEGIN_MSG_MAP_EX(AddMagnetLinkDialog)
        MSG_WM_INITDIALOG(OnInitDialog)
        COMMAND_ID_HANDLER_EX(ID_MAGNET_ADD_LINKS, OnAddMagnetLinks)
    END_MSG_MAP()

	std::vector<std::wstring> m_links;
	std::vector<libtorrent::add_torrent_params> m_params;
};
}
