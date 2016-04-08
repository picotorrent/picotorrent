#include <picotorrent/client/environment.hpp>

#include <picotorrent/client/string_operations.hpp>
#include <picotorrent/core/pal.hpp>

#include <shlobj.h>
#include <shlwapi.h>

using picotorrent::client::environment;
using picotorrent::core::pal;

std::string environment::get_data_path()
{
    if (is_installed())
    {
        std::string app_data = get_special_folder(client::local_app_data);
        return pal::combine_paths(app_data, "PicoTorrent");
    }

    TCHAR buf[MAX_PATH];
    GetModuleFileName(NULL, buf, ARRAYSIZE(buf));
    PathRemoveFileSpec(buf);

    return to_string(buf);
}

std::string environment::get_special_folder(picotorrent::client::special_folder folder)
{
    PWSTR buf;
    GUID g;

    switch (folder)
    {
    case picotorrent::client::user_downloads:
        g = FOLDERID_Downloads;
        break;
    case picotorrent::client::local_app_data:
        g = FOLDERID_LocalAppData;
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
