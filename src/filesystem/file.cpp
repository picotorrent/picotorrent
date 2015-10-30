#include <picotorrent/filesystem/file.hpp>

#include <picotorrent/filesystem/path.hpp>
#include <picotorrent/logging/log.hpp>
#include <windows.h>

namespace fs = picotorrent::filesystem;
using fs::file;

file::file(const fs::path &p)
    : p_(p)
{
}

fs::path& file::path()
{
    return p_;
}

void file::read_all(std::vector<char> &buf)
{
    buf.clear();

    HANDLE hFile = CreateFile(
        p_.to_string().c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        throw new std::runtime_error("Could not open file.");
    }

    DWORD read = 0;

    do
    {
        char tmp[1024];
        read = 0;

        if (!ReadFile(
            hFile,
            tmp,
            _ARRAYSIZE(tmp),
            &read,
            NULL))
        {
            CloseHandle(hFile);
            throw new std::runtime_error("Error when reading file.");
        }

        buf.insert(buf.end(), tmp, tmp + read);
    } while (read > 0);

    CloseHandle(hFile);
}

void file::write_all(const std::vector<char> &buf)
{
    HANDLE hFile = CreateFile(
        p_.to_string().c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        throw new std::runtime_error("Could not open file for writing.");
    }

    DWORD written;

    if (!WriteFile(
        hFile,
        &buf[0],
        buf.size(),
        &written,
        NULL))
    {
        CloseHandle(hFile);
        throw new std::runtime_error("Error when writing to file.");
    }

    CloseHandle(hFile);
    hFile = NULL;
}
