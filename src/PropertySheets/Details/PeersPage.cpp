#include "PeersPage.hpp"

#include <libtorrent/peer_info.hpp>
#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <strsafe.h>

#include "../../Configuration.hpp"
#include "../../Models/Peer.hpp"
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
}

BOOL PeersPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    // Set up UI
    m_peerList = std::make_unique<UI::PeerListView>(GetDlgItem(ID_DETAILS_PEERS_LIST));
    return FALSE;
}

BOOL PeersPage::OnKillActive()
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_UNREGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    // Clear current state (if any)
    m_state.clear();
    m_peerList->SetItemCount(0);

    return TRUE;
}

BOOL PeersPage::OnSetActive()
{
    // Add current peers
    std::vector<lt::peer_info> peers;
    m_torrent.get_peer_info(peers);

    for (auto& peer : peers)
    {
        std::stringstream ss;
        ss << peer.ip;
        std::wstring endpoint = TWS(ss.str());

        Models::Peer p = Models::Peer::Map(peer);
        m_peerList->Add(p);

        // Add the peer to our state, setting the value to 'true'
        // which means we've updated it
        m_state.insert({ endpoint, true });
    }

    m_peerList->SetItemCount((int)peers.size());

    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_REGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    return TRUE;
}

LRESULT PeersPage::OnTorrentUpdated(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    lt::sha1_hash* hash = reinterpret_cast<lt::sha1_hash*>(lParam);
    if (*hash != m_torrent.info_hash()) { return FALSE; }

    // Set all state bool to 'false'
    std::for_each(
        m_state.begin(),
        m_state.end(),
        [this](std::pair<std::wstring, bool> p) { m_state.at(p.first) = false; });

    std::vector<lt::peer_info> peers;
    m_torrent.get_peer_info(peers);

    for (auto& peer : peers)
    {
        std::stringstream ss;
        ss << peer.ip;
        std::wstring endpoint = TWS(ss.str());

        Models::Peer p = Models::Peer::Map(peer);

        if (m_state.find(endpoint) == m_state.end())
        {
            m_state.insert({ endpoint, false });
            m_peerList->Add(p);
        }
        else
        {
            m_peerList->Update(p);
        }

        m_state.at(endpoint) = true;
    }

    // Find all peers which haven't been updated
    std::vector<std::wstring> rem;
    std::for_each(
        m_state.begin(),
        m_state.end(),
        [&rem](const std::pair<std::wstring, bool>& p) { if (!p.second) { rem.push_back(p.first); } });

    for (auto& r : rem)
    {
        Models::Peer p = Models::Peer::Map(r);

        m_peerList->Remove(p);
        m_state.erase(r);
    }

    m_peerList->SetItemCount((int)peers.size());

    return FALSE;
}
