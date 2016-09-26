#pragma once

#include <string>
#include <vector>

#include <windows.h>

namespace Sources
{
    struct Source
    {
        struct PreviewItem
        {
            std::wstring name;
            std::wstring savePath;
            int64_t size;
        };

        virtual HWND GetWindowHandle(HINSTANCE hInstance, HWND hWndParent) = 0;
        virtual std::vector<PreviewItem> GetPreview() = 0;
        virtual std::wstring GetName() = 0;
        virtual void Import() = 0;
    };
}
