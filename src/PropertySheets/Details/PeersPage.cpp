#include "PeersPage.hpp"

#include <libtorrent/peer_info.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <strsafe.h>

#include "../../Configuration.hpp"
#include "../../resources.h"
#include "../../Translator.hpp"
#include "../../UI/PeerListView.hpp"

namespace lt = libtorrent;
using PropertySheets::Details::PeersPage;

PeersPage::PeersPage(const libtorrent::torrent_handle& th)
    : m_torrent(th)
{
    m_title = TRW("peers");
    SetTitle(m_title.c_str());
}

void PeersPage::OnDestroy()
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_UNREGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);
}

BOOL PeersPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    // Set up UI
    m_peerList = std::make_unique<UI::PeerListView>(GetDlgItem(ID_DETAILS_PEERS_LIST));

    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_REGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    return FALSE;
}

LRESULT PeersPage::OnTorrentUpdated(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    lt::sha1_hash* hash = reinterpret_cast<lt::sha1_hash*>(lParam);
    if (*hash != m_torrent.info_hash()) { return FALSE; }

    std::vector<lt::peer_info> peers;
    m_torrent.get_peer_info(peers);

    return FALSE;
}
