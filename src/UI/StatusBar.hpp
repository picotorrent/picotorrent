#pragma once

#include "../stdafx.h"

#include <stdint.h>

namespace UI
{
class StatusBar
{
public:
    HWND Create(HWND hWndParent, RECT rc);

    void SetDhtNodes(int64_t nodes);
    void SetTorrentCount(int total, int selected);
    void SetTransferRates(int dl, int ul);

private:
    CStatusBarCtrl m_status;
};
}
