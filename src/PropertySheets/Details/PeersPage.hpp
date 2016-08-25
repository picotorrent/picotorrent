#pragma once

#include "../../resources.h"
#include "../../stdafx.h"

#include <memory>
#include <string>

namespace libtorrent
{
    struct torrent_handle;
}

namespace UI
{
    class PeerListView;
}

namespace PropertySheets
{
namespace Details
{
    class PeersPage : public CPropertyPageImpl<PeersPage>
    {
        friend class CPropertyPageImpl<PeersPage>;

    public:
        enum { IDD = IDD_DETAILS_PEERS };
        PeersPage(const libtorrent::torrent_handle& th);

    private:
        void OnDestroy();
        BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
        LRESULT OnTorrentUpdated(UINT uMsg, WPARAM wParam, LPARAM lParam);

        BEGIN_MSG_MAP_EX(PeersPage)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_INITDIALOG(OnInitDialog)

            MESSAGE_HANDLER_EX(PT_TORRENT_UPDATED, OnTorrentUpdated)

            CHAIN_MSG_MAP(CPropertyPageImpl<PeersPage>)
        END_MSG_MAP()

        std::wstring m_title;
        std::unique_ptr<UI::PeerListView> m_peerList;
        const libtorrent::torrent_handle& m_torrent;
    };
}
}
