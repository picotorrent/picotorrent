#pragma once

#include "../stdafx.h"

namespace UI
{
    class ComboBox : public CComboBox
    {
    public:
        using CComboBox::CComboBox;

        inline ComboBox& operator=(const HWND hWnd)
        {
            m_hWnd = hWnd;
            return *this;
        }

        template<typename T>
        inline T GetSelectedItemData()
        {
            return static_cast<T>(GetItemData(GetCurSel()));
        }
    };
}
