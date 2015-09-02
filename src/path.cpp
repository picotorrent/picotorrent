#include "path.h"

#include <boost/filesystem.hpp>
#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

#include "env.h"

namespace fs = boost::filesystem;
using namespace pico;

std::wstring GetRoot()
{
    if (Env::IsInstalled())
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

fs::path Path::GetDefaultDownloadsPath()
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

fs::path Path::GetLogPath()
{
    std::wstring root = GetRoot();

    TCHAR ret[MAX_PATH];
    return PathCombine(ret, root.c_str(), L"logs");
}

fs::path Path::GetStatePath()
{
    std::wstring root = GetRoot();

    TCHAR ret[MAX_PATH];
    return PathCombine(ret, root.c_str(), L".session_state");
}

fs::path Path::GetTorrentsPath()
{
    std::wstring root = GetRoot();

    TCHAR ret[MAX_PATH];
    return PathCombine(ret, root.c_str(), L"torrents");
}
