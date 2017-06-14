#include "StatusBar.hpp"

#include <strsafe.h>

#include "../Scaler.hpp"

using UI::StatusBar;

HWND StatusBar::Create(HWND hWndParent, RECT rc)
{
    HWND hWnd = m_status.Create(
        hWndParent,
        rc,
        NULL,
        WS_CHILD | WS_VISIBLE);

    int parts[] = { SX(120), SX(300), -1 };
    m_status.SetParts(ARRAYSIZE(parts), parts);

    return hWnd;
}

void StatusBar::SetDhtNodes(int64_t nodes)
{
    TCHAR st[1024];
    StringCchPrintf(st, ARRAYSIZE(st), TEXT("DHT: %d node(s)"), nodes);
    m_status.SetText(2, st);
}

void StatusBar::SetTorrentCount(int total, int selected)
{
    TCHAR st[1024];
    StringCchPrintf(st, ARRAYSIZE(st), TEXT("%d torrent(s)"), total);
    m_status.SetText(0, st);
}

void StatusBar::SetTransferRates(int dl, int ul)
{
    TCHAR dl_str[1024];
    StrFormatByteSize64(dl, dl_str, ARRAYSIZE(dl_str));
    TCHAR ul_str[1024];
    StrFormatByteSize64(ul, ul_str, ARRAYSIZE(ul_str));

    TCHAR str[1024];
    StringCchPrintf(str, ARRAYSIZE(str), TEXT("DL: %s/s, UL: %s/s"), dl_str, ul_str);

    m_status.SetText(1, str);
}
