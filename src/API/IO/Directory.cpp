#include "Directory.hpp"

#include <shellapi.h>
#include <Shlwapi.h>

#include "File.hpp"

using API::IO::Directory;

Directory::Directory(DirectoryPath const& path)
    : m_path(path)
{
}

bool Directory::Exists()
{
    DWORD dwAttr = GetFileAttributes(m_path);
    return (dwAttr != INVALID_FILE_ATTRIBUTES && dwAttr & FILE_ATTRIBUTE_DIRECTORY);
}

FileHandleCollection Directory::GetFiles(std::wstring const& filter)
{
    TCHAR p[MAX_PATH];
    PathCombine(p, m_path, filter.c_str());

    FileHandleCollection res;

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFileEx(
        p,
        FindExInfoBasic,
        &ffd,
        FindExSearchNameMatch,
        NULL,
        FIND_FIRST_EX_LARGE_FETCH);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return res;
    }

    do
    {
        if (StrCmp(ffd.cFileName, L".") == 0
            || StrCmp(ffd.cFileName, L"..") == 0)
        {
            continue;
        }

        TCHAR p2[MAX_PATH];
        PathCombine(p2, m_path, ffd.cFileName);

        FileHandle handle = std::make_shared<File>(p2);
        res.push_back(handle);

    } while (FindNextFile(hFind, &ffd) > 0);

    FindClose(hFind);

    return res;
}
