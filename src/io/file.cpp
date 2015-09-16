#include "file.h"

#include <boost/log/trivial.hpp>
#include <windows.h>
#include "../util.h"

using namespace pico::io;

void File::Delete(const std::wstring &path)
{
    if (!DeleteFile(path.c_str()))
    {
        BOOST_LOG_TRIVIAL(error)
            << "Could not delete file " << path
            << ", error: " << GetLastError();
    }
}

bool File::Exists(const std::wstring& path)
{
    DWORD attr = GetFileAttributes(path.c_str());

    return (attr != INVALID_FILE_ATTRIBUTES
        && !(attr & FILE_ATTRIBUTE_DIRECTORY));
}

bool File::ReadBuffer(const std::string& path, std::vector<char>& buffer)
{
    return ReadBuffer(Util::ToWideString(path), buffer);
}

bool File::ReadBuffer(const std::wstring& path, std::vector<char>& buffer)
{
    HANDLE hFile = CreateFile(path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        BOOST_LOG_TRIVIAL(error)
            << "Could not open file '"
            << path << "' for reading, error: "
            << GetLastError();
        return false;
    }

    buffer.clear();

    char buf[1024];
    DWORD read;

    do
    {
        if (!ReadFile(hFile,
            &buf,
            _ARRAYSIZE(buf),
            &read,
            NULL))
        {
            BOOST_LOG_TRIVIAL(error)
                << "Could not read file '"
                << path << "', error: "
                << GetLastError();
            return false;
        }

        buffer.insert(buffer.end(), buf, buf + read);
    } while (read > 0);

    return true;
}

bool File::WriteBuffer(const std::string& path, const std::vector<char>& buffer)
{
    return WriteBuffer(Util::ToWideString(path), buffer);
}

bool File::WriteBuffer(const std::wstring& path, const std::vector<char>& buffer)
{
    HANDLE hFile = CreateFile(path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        BOOST_LOG_TRIVIAL(error)
            << "Could not open file '"
            << path << "' for writing, error: "
            << GetLastError();
        return false;
    }

    if (!WriteFile(hFile,
        &buffer[0],
        (DWORD)buffer.size(),
        NULL,
        NULL))
    {
        BOOST_LOG_TRIVIAL(error)
            << "Could not write file '"
            << path << "', error: "
            << GetLastError();
        return false;
    }

    return true;
}
