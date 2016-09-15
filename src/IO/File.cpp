#include "File.hpp"

#include <windows.h>

#include <memory>

using IO::File;

struct CloseHandleDeleter
{
    typedef HANDLE pointer;
    void operator()(HANDLE h) { CloseHandle(h); }
};

void File::Delete(const std::wstring& path)
{
    DeleteFile(path.c_str());
}

bool File::Exists(const std::wstring &path)
{
    DWORD dwAttr = GetFileAttributes(path.c_str());
    return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

std::vector<char> File::ReadAllBytes(const std::wstring &path, std::error_code& ec)
{
    std::unique_ptr<HANDLE, CloseHandleDeleter> handle(CreateFile(
        path.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL));

    if (handle.get() == INVALID_HANDLE_VALUE)
    {
        ec.assign(GetLastError(), std::system_category());
        return std::vector<char>();
    }

    std::vector<char> result;
    DWORD read = 0;

    do
    {
        char tmp[1024];
        read = 0;

        if (!ReadFile(
            handle.get(),
            tmp,
            ARRAYSIZE(tmp),
            &read,
            NULL))
        {
            ec.assign(GetLastError(), std::system_category());
            return std::vector<char>();
        }

        result.insert(
            result.end(),
            tmp,
            tmp + read);
    } while (read > 0);

    return result;
}

void File::WriteAllBytes(const std::wstring& path, const std::vector<char>& buf, std::error_code& ec)
{
    std::unique_ptr<HANDLE, CloseHandleDeleter> handle(CreateFile(
        path.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL));

    if (handle.get() == INVALID_HANDLE_VALUE)
    {
        ec.assign(GetLastError(), std::system_category());
        return;
    }

    if (!WriteFile(
        handle.get(),
        &buf[0],
        (DWORD)buf.size(),
        NULL,
        NULL))
    {
        ec.assign(GetLastError(), std::system_category());
    }
}
