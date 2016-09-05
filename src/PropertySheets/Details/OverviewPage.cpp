#include "OverviewPage.hpp"

#include <libtorrent/sha1_hash.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>

#include <strsafe.h>

#include "../../Configuration.hpp"
#include "../../Models/Torrent.hpp"
#include "../../resources.h"
#include "../../Translator.hpp"

namespace lt = libtorrent;
using PropertySheets::Details::OverviewPage;

OverviewPage::OverviewPage(const lt::torrent_handle& th)
    : m_torrent(th)
{
    m_title = TRW("overview");
    SetTitle(m_title.c_str());
}

BOOL OverviewPage::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    SetDlgItemText(ID_OVERVIEW_STATISTICS_GROUP, TRW("statistics"));
    SetDlgItemText(ID_OVERVIEW_RATIO_TEXT, TRW("ratio"));
    SetDlgItemText(ID_OVERVIEW_PIECES_TEXT, TRW("pieces"));
    SetDlgItemText(ID_OVERVIEW_DOWNLOADED_TEXT, TRW("downloaded"));
    SetDlgItemText(ID_OVERVIEW_UPLOADED_TEXT, TRW("uploaded"));

    return FALSE;
}

BOOL OverviewPage::OnKillActive()
{
    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_UNREGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    return TRUE;
}

BOOL OverviewPage::OnSetActive()
{
    Update();

    ::SendMessage(
        ::GetParent(::GetParent(m_hWnd)),
        PT_REGISTERNOTIFY,
        NULL,
        (LPARAM)m_hWnd);

    return TRUE;
}

LRESULT OverviewPage::OnTorrentUpdated(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    lt::sha1_hash* hash = reinterpret_cast<lt::sha1_hash*>(lParam);
    if (*hash != m_torrent.info_hash()) { return FALSE; }

    Update();

    return FALSE;
}

void OverviewPage::Update()
{
    Models::Torrent t = Models::Torrent::Map(m_torrent.status());

    // Share ratio
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(3) << t.shareRatio;
    SetDlgItemText(ID_OVERVIEW_RATIO, ss.str().c_str());

    // Pieces
    TCHAR p_str[100];
    TCHAR p_bytes[100];
    StrFormatByteSize64(t.pieceLength, p_bytes, ARRAYSIZE(p_bytes));
    StringCchPrintf(p_str, ARRAYSIZE(p_str), TEXT("%d x %s (have %d)"), t.piecesCount, p_bytes, t.piecesHave);
    SetDlgItemText(ID_OVERVIEW_PIECES, p_str);

    // DL
    TCHAR dl_str[100];
    StrFormatByteSize64(t.downloadedBytes, dl_str, ARRAYSIZE(dl_str));
    SetDlgItemText(ID_OVERVIEW_DOWNLOADED, dl_str);

    // UL
    TCHAR ul_str[100];
    StrFormatByteSize64(t.uploadedBytes, ul_str, ARRAYSIZE(ul_str));
    SetDlgItemText(ID_OVERVIEW_UPLOADED, ul_str);
}
