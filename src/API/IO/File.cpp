#include "File.hpp"

#include <shlwapi.h>
#include <strsafe.h>

using API::IO::File;

struct CloseHandleDeleter
{
    typedef HANDLE pointer;
    void operator()(HANDLE h) { CloseHandle(h); }
};

File::File(FilePath const& path)
    : m_path(path)
{
}

FileHandle File::ChangeExtension(std::wstring const& extension)
{
    TCHAR np[MAX_PATH];
    StringCchCopy(np, ARRAYSIZE(np), m_path);
    PathRenameExtension(np, extension.c_str());
    return std::make_shared<File>(np);
}

void File::Delete()
{
    DeleteFile(m_path);
}

bool File::Exists()
{
    DWORD dwAttr = GetFileAttributes(m_path);
    return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

size_t File::Length()
{
    std::unique_ptr<HANDLE, CloseHandleDeleter> handle(CreateFile(
        m_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL));

    LARGE_INTEGER size;
    
    if (GetFileSizeEx(handle.get(), &size))
    {
        return static_cast<size_t>(size.QuadPart);
    }
    
    return -1;
}

std::vector<char> File::ReadAllBytes(std::error_code& ec)
{
    std::unique_ptr<HANDLE, CloseHandleDeleter> handle(CreateFile(
        m_path,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL));

    if (handle.get() == INVALID_HANDLE_VALUE)
    {
        ec.assign(GetLastError(), std::system_category());
        return ByteBuffer();
    }

    ByteBuffer result;
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
            return ByteBuffer();
        }

        result.insert(
            result.end(),
            tmp,
            tmp + read);
    } while (read > 0);

    return result;
}
