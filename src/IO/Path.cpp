#include "Path.hpp"

#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>

using IO::Path;

std::wstring Path::Combine(const std::wstring& path1, const std::wstring& path2)
{
    TCHAR res[MAX_PATH];
    PathCombine(res, path1.c_str(), path2.c_str());
    return res;
}

std::wstring Path::ReplaceExtension(const std::wstring& path, const std::wstring& ext)
{
    TCHAR p[MAX_PATH];
    StringCchCopy(p, ARRAYSIZE(p), path.c_str());
    PathRemoveExtension(p);
    PathAddExtension(p, ext.c_str());
    return p;
}
