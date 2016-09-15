#pragma once

#include "../stdafx.h"

namespace UI
{
    class CheckBox : public CButton
    {
    public:
        using CButton::CButton;

        inline CheckBox& operator=(const HWND hWnd)
        {
            m_hWnd = hWnd;
            return *this;
        }

        void Check();
        bool IsChecked();
        void Toggle();
    };
}
