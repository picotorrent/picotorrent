#include <new>

#include "Clipboard.hpp"

#include <Windows.h>

#include "StringUtils.hpp"

void Clipboard::Set(const std::wstring& content)
{
    const size_t content_len = (content.size() + 1) * sizeof(wchar_t);

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, content_len);

    if (hMem == nullptr) throw std::bad_alloc();

    memcpy(GlobalLock(hMem), content.c_str(), content_len);
    GlobalUnlock(hMem);

    OpenClipboard(NULL);
    EmptyClipboard();
    if (SetClipboardData(CF_UNICODETEXT, hMem) == NULL) {
        //We need to free memory only on failure
        //as otherwise system owns it.
        GlobalFree(hMem);
    }

    CloseClipboard();
}
