#include <picotorrent/filesystem/directory.hpp>

#include <windows.h>

namespace fs = picotorrent::filesystem;
using fs::directory;

void create_directories(const std::wstring &directory)
{
    static const std::wstring separators(L"\\/");

    // If the specified directory name doesn't exist, do our thing
    DWORD fileAttributes = ::GetFileAttributesW(directory.c_str());

    if (fileAttributes == INVALID_FILE_ATTRIBUTES)
    {
        // Recursively do it all again for the parent directory, if any
        std::size_t slashIndex = directory.find_last_of(separators);

        if (slashIndex != std::wstring::npos)
        {
            create_directories(directory.substr(0, slashIndex));
        }

        // Create the last directory on the path (the recursive calls will have taken
        // care of the parent directories by now)
        BOOL result = ::CreateDirectory(directory.c_str(), nullptr);

        if (result == FALSE)
        {
            throw std::runtime_error("Could not create directory");
        }

    }
    else
    {
        // Specified directory name already exists as a file or directory
        bool isDirectoryOrJunction =
            ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
            ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

        if (!isDirectoryOrJunction)
        {
            throw std::runtime_error("Could not create directory because a file with the same name exists");
        }
    }
}

directory::directory(const fs::path &p)
    : p_(p)
{
}

void directory::create()
{
    create_directories(p_.to_string());
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
