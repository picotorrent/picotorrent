#include "path.h"

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

#include "../env.h"

using namespace pico::io;

std::wstring GetRoot()
{
    if (pico::Env::IsInstalled())
    {
        LPWSTR appData = NULL;
        HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &appData);

        if (FAILED(hr))
        {
            // TODO(log)
            return L"";
        }

        TCHAR ret[MAX_PATH];
        return PathCombine(ret, appData, L"PicoTorrent");
    }
    else
    {
        TCHAR curr[MAX_PATH];

        if (!GetCurrentDirectory(MAX_PATH, curr))
        {
            // TODO(log)
            return L"";
        }

        return curr;
    }
}

std::wstring Path::ChangeExtension(const std::wstring &file, const std::wstring &extension)
{
    TCHAR buf[MAX_PATH];
    wcscpy_s(buf, _ARRAYSIZE(buf), file.c_str());

    if (!PathRenameExtension(buf, extension.c_str()))
    {
        // TODO(log)
    }

    return buf;
}

std::wstring Path::Combine(const std::wstring& path1, const std::wstring& path2)
{
    TCHAR ret[MAX_PATH];
    return PathCombine(ret, path1.c_str(), path2.c_str());
}

std::wstring Path::GetDefaultDownloadsPath()
{
    LPWSTR dlPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &dlPath);

    if (FAILED(hr))
    {
        // TODO(log)
        return L"";
    }

    return dlPath;
}

std::wstring Path::GetExtension(const std::wstring &file)
{
    return PathFindExtension(file.c_str());
}

std::wstring Path::GetLogPath()
{
    std::wstring root = GetRoot();

    TCHAR ret[MAX_PATH];
    return PathCombine(ret, root.c_str(), L"logs");
}

std::wstring Path::GetStatePath()
{
    std::wstring root = GetRoot();

    TCHAR ret[MAX_PATH];
    return PathCombine(ret, root.c_str(), L".session_state");
}

std::wstring Path::GetTorrentsPath()
{
    std::wstring root = GetRoot();

    TCHAR ret[MAX_PATH];
    return PathCombine(ret, root.c_str(), L"torrents");
}
