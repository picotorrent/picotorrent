#include "Clipboard.hpp"

#include <Windows.h>

#include "StringUtils.hpp"

void Clipboard::Set(const std::wstring& content)
{
    std::string s = ToString(content);

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, s.size() + 1);
    memcpy(GlobalLock(hMem), s.c_str(), s.size());
    GlobalUnlock(hMem);

    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();

    GlobalFree(hMem);
}
