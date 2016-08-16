#pragma once

#include "../stdafx.h"

namespace UI
{
class StatusBar
{
public:
    HWND Create(HWND hWndParent, RECT rc);

private:
    CStatusBarCtrl m_status;
};
}
