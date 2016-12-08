#pragma once

#include <string>

#include "../stdafx.h"

namespace UI
{
    class TextBox : public CEdit
    {
    public:
        using CEdit::CEdit;

        inline TextBox& operator=(const HWND hWnd)
        {
            m_hWnd = hWnd;
            return *this;
        }

        std::string GetValueA();
        std::wstring GetValueW();
    };
}
