#include "StatusBar.hpp"

using UI::StatusBar;

HWND StatusBar::Create(HWND hWndParent, RECT rc)
{
    HWND hWnd = m_status.Create(
        hWndParent,
        rc,
        NULL,
        WS_CHILD | WS_VISIBLE);

    RECT src;
    m_status.GetClientRect(&src);

    int parts[4] = {
        -1,
        src.right - 200,
        src.right - 100,
        src.right - 50
    };
    
    m_status.SetParts(4, parts);

    return hWnd;
}
