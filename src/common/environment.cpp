#include <picotorrent/common/environment.hpp>

#include <picotorrent/common/command_line.hpp>
#include <picotorrent/common/string_operations.hpp>
#include <picotorrent/core/pal.hpp>

#include <shlobj.h>
#include <shlwapi.h>

using picotorrent::common::command_line;
using picotorrent::common::environment;
using picotorrent::core::pal;

std::string environment::get_data_path()
{
    if (is_installed())
    {
        special_folder f = is_running_as_windows_service()
            ? special_folder::program_data
            : special_folder::local_app_data;

        std::string app_data = get_special_folder(f);
        return pal::combine_paths(app_data, "PicoTorrent");
    }

    TCHAR buf[MAX_PATH];
    GetModuleFileName(NULL, buf, ARRAYSIZE(buf));
    PathRemoveFileSpec(buf);

    return to_string(buf);
}

std::string environment::get_special_folder(picotorrent::common::special_folder folder)
{
    PWSTR buf;
    GUID g;

    switch (folder)
    {
    case special_folder::user_downloads:
        g = FOLDERID_Downloads;
        break;
    case special_folder::local_app_data:
        g = FOLDERID_LocalAppData;
        break;
    case special_folder::public_downloads:
        g = FOLDERID_PublicDownloads;
        break;
    case special_folder::program_data:
        g = FOLDERID_ProgramData;
        break;
    }

    HRESULT hr = SHGetKnownFolderPath(
        g,
        0,
        NULL,
        &buf);

    std::string p = to_string(buf);
    CoTaskMemFree(buf);

    return p;
}

std::string environment::get_temporary_directory()
{
    TCHAR p[MAX_PATH];
    GetTempPath(ARRAYSIZE(p), p);
    return to_string(p);
}

bool environment::is_installed()
{
    HKEY hKey = NULL;

    LSTATUS lStat = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        L"Software\\PicoTorrent",
        0,
        KEY_READ,
        &hKey);

    if (lStat != ERROR_SUCCESS)
    {
        if (hKey != NULL) { RegCloseKey(hKey); }
        return false;
    }

    TCHAR val[MAX_PATH];
    DWORD bufSize = MAX_PATH;

    lStat = RegQueryValueEx(
        hKey,
        L"InstallDirectory",
        NULL,
        NULL,
        (LPBYTE)val,
        &bufSize);

    if (lStat != ERROR_SUCCESS)
    {
        if (hKey != NULL) { RegCloseKey(hKey); }
        return false;
    }

    std::wstring installDir(val);
    GetModuleFileName(NULL, val, MAX_PATH);

    if (installDir + L"PicoTorrent.exe" == val)
    {
        if (hKey != NULL) { RegCloseKey(hKey); }
        return true;
    }

    if (hKey != NULL) { RegCloseKey(hKey); }
    return false;
}

bool environment::is_running_as_windows_service()
{
    command_line cmd = command_line::parse(GetCommandLine(), true);
    return cmd.daemon();
}
