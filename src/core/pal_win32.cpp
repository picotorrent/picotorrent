#include <picotorrent/core/pal.hpp>

#include <Windows.h>
#include <Shlwapi.h>
#include <strsafe.h>

using picotorrent::core::pal;

std::string from_wide(const std::wstring &str)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &result[0], size, NULL, NULL);
    return result;
}

std::wstring to_wide(const std::string &str)
{
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring result(size, '\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

std::string pal::combine_paths(const std::string &p1, const std::string &p2)
{
    TCHAR res[MAX_PATH];
    if (PathCombine(res, to_wide(p1).c_str(), to_wide(p2).c_str()) == NULL)
    {
        // TODO(error)
        return std::string();
    }
    return from_wide(res);
}

bool pal::directory_exists(const std::string &path)
{
    DWORD dwAttrib = GetFileAttributes(to_wide(path).c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool pal::file_exists(const std::string &path)
{
    DWORD dwAttrib = GetFileAttributes(to_wide(path).c_str());
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

std::vector<std::string> pal::get_directory_entries(const std::string &dir, const std::string &filter)
{
    std::vector<std::string> res;

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFileEx(
        to_wide(combine_paths(dir, filter)).c_str(),
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
        res.push_back(combine_paths(dir, from_wide(ffd.cFileName)));
    } while (FindNextFile(hFind, &ffd) > 0);

    FindClose(hFind);

    return res;
}

void pal::remove_file(const std::string &path)
{
    if (!DeleteFile(to_wide(path).c_str()))
    {
        // TODO(error)
    }
}

std::string pal::replace_extension(const std::string &path, const std::string &new_extension)
{
    TCHAR p[MAX_PATH];
    StringCchCopy(p, ARRAYSIZE(p), to_wide(path).c_str());
    PathRemoveExtension(p);
    PathAddExtension(p, to_wide(new_extension).c_str());
    return from_wide(p);
}
