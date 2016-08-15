#include "Directory.hpp"

#include <windows.h>
#include <shlwapi.h>

#include "Path.hpp"

using IO::Directory;

void Directory::Create(const std::wstring& dir)
{
    static const std::wstring separators(L"\\/");

    // If the specified directory name doesn't exist, do our thing
    DWORD fileAttributes = ::GetFileAttributesW(dir.c_str());

    if (fileAttributes == INVALID_FILE_ATTRIBUTES)
    {
        // Recursively do it all again for the parent directory, if any
        std::size_t slashIndex = dir.find_last_of(separators);

        if (slashIndex != std::wstring::npos)
        {
            Create(dir.substr(0, slashIndex));
        }

        // Create the last directory on the path (the recursive calls will have taken
        // care of the parent directories by now)
        ::CreateDirectory(dir.c_str(), nullptr);
    }
}

bool Directory::Exists(const std::wstring& path)
{
    DWORD dwAttr = GetFileAttributes(path.c_str());
    return (dwAttr != INVALID_FILE_ATTRIBUTES && dwAttr & FILE_ATTRIBUTE_DIRECTORY);
}

std::vector<std::wstring> Directory::GetFiles(const std::wstring& path, const std::wstring& filter)
{
    std::vector<std::wstring> res;

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFileEx(
        Path::Combine(path, filter).c_str(),
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
        if (StrCmp(ffd.cFileName, L".") != 0
            && StrCmp(ffd.cFileName, L"..") != 0)
        {
            res.push_back(Path::Combine(path, ffd.cFileName));
        }
    } while (FindNextFile(hFind, &ffd) > 0);

    FindClose(hFind);

    return res;
}
