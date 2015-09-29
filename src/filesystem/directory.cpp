#include <picotorrent/filesystem/directory.hpp>

#include <windows.h>

namespace fs = picotorrent::filesystem;
using fs::directory;

directory::directory(const fs::path &p)
    : p_(p)
{
}

void directory::create()
{
    CreateDirectory(p_.to_string().c_str(), NULL);
}

fs::path& directory::path()
{
    return p_;
}

std::vector<fs::path> directory::get_files(const fs::path &p)
{
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFileEx(
        p.to_string().c_str(),
        FindExInfoBasic,
        &ffd,
        FindExSearchNameMatch,
        NULL,
        FIND_FIRST_EX_LARGE_FETCH);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();

        switch (err)
        {
        case ERROR_FILE_NOT_FOUND:
            return std::vector<fs::path>();
        }

        throw new std::runtime_error("FindFirstFileEx");
    }

    std::vector<fs::path> paths;

    do
    {
        fs::path p(ffd.cFileName);
        paths.push_back(p_.combine(p));
    } while (FindNextFile(hFind, &ffd) > 0);

    FindClose(hFind);

    return paths;
}
