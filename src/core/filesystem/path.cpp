#include <picotorrent/core/filesystem/path.hpp>

#include <shlwapi.h>
#include <strsafe.h>

using picotorrent::core::filesystem::path;

path::path(const std::wstring &p)
    : p_(p)
{
}

path::path(const wchar_t *p)
    : p_(p)
{
}

path path::combine(const path &p) const
{
    TCHAR combined[MAX_PATH];
    PathCombine(combined, p_.c_str(), p.to_string().c_str());
    return path(combined);
}

bool path::exists() const
{
    return PathFileExists(p_.c_str()) == TRUE ? true : false;
}

path path::filename() const
{
    return path(PathFindFileName(p_.c_str()));
}

bool path::is_directory()
{
    return false;
}

bool path::is_file()
{
    return false;
}

path path::replace_extension(const std::wstring &ext)
{
    TCHAR p[MAX_PATH];
    StringCchCopy(p, MAX_PATH, p_.c_str());

    PathRemoveExtension(p);
    PathAddExtension(p, ext.c_str());

    return path(p);
}

std::wstring path::to_string() const
{
    return p_;
}
