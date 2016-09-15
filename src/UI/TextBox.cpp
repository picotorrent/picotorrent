#include "TextBox.hpp"

#include <vector>

#include "../StringUtils.hpp"

using UI::TextBox;

std::string TextBox::GetValueA()
{
    return ToString(GetValueW());
}

std::wstring TextBox::GetValueW()
{
    int length = GetWindowTextLength();
    std::vector<wchar_t> t(length + 1);
    GetWindowText(&t[0], (int)t.size());

    return &t[0];
}

