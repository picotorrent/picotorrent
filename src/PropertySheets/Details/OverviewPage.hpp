#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <string>

namespace libtorrent
{
    struct torrent_handle;
}

namespace PropertySheets
{
namespace Details
{
    class OverviewPage : public CPropertyPageImpl<OverviewPage>
    {
        friend class CPropertyPageImpl<OverviewPage>;

    public:
        enum { IDD = IDD_DETAILS_OVERVIEW };
        OverviewPage(const libtorrent::torrent_handle& th);

    private:
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        BOOL OnKillActive();
        BOOL OnSetActive();
        LRESULT OnTorrentUpdated(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void Update();

        BEGIN_MSG_MAP_EX(OverviewPage)
            MSG_WM_INITDIALOG(OnInitDialog)
            MESSAGE_HANDLER_EX(PT_TORRENT_UPDATED, OnTorrentUpdated)
            CHAIN_MSG_MAP(CPropertyPageImpl<OverviewPage>)
        END_MSG_MAP()

        std::wstring m_title;
        const libtorrent::torrent_handle& m_torrent;
    };
}
}
