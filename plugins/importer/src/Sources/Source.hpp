#pragma once

#include <string>
#include <vector>

#include <windows.h>

namespace Sources
{
    struct Source
    {
        virtual HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent) = 0;
        virtual std::vector<std::wstring> GetPreview() = 0;
        virtual std::wstring GetName() = 0;
    };
}
