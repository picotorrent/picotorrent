#include  "../platform.h"

#include <windows.h>
#include <shlobj.h>

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string Platform::GetDownloadsPath()
{
    LPWSTR path;

    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads,
        0,
        NULL,
        &path);

    if (SUCCEEDED(hr))
    {
        return utf8_encode(path);
    }

    return "";
}
