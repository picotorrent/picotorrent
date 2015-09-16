#include "directory.h"

#include <windows.h>

#include "path.h"

using namespace pico::io;

void Directory::CreateDirectories(const std::wstring &path)
{
    if (!CreateDirectory(path.c_str(), NULL))
    {
        // TODO(log)
    }
}

bool Directory::Exists(const std::wstring &path)
{
    DWORD attr = GetFileAttributes(path.c_str());

    return (attr != INVALID_FILE_ATTRIBUTES
        && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

std::vector<std::wstring> Directory::GetFiles(const std::wstring& path, const std::wstring &filter)
{
    std::wstring filteredPath = Path::Combine(path, filter);

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(filteredPath.c_str(), &ffd);

    std::vector<std::wstring> files;

    if (hFind == INVALID_HANDLE_VALUE)
    {
        // TODO(log)
        return files;
    }

    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            continue;
        }

        files.push_back(Path::Combine(path, ffd.cFileName));
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);

    return files;
}
