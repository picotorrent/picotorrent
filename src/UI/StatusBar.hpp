#pragma once

#include "../stdafx.h"

namespace UI
{
class StatusBar
{
public:
    HWND Create(HWND hWndParent, RECT rc);

    void SetDhtNodes(int nodes);
    void SetTorrentCount(int total, int selected);
    void SetTransferRates(int dl, int ul);

private:
    CStatusBarCtrl m_status;
};
}
