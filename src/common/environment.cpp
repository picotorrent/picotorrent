#include <picotorrent/common/environment.hpp>

#include <picotorrent/filesystem/path.hpp>
#include <shlobj.h>

namespace fs = picotorrent::filesystem;
using picotorrent::common::environment;

fs::path environment::get_data_path()
{
    if (is_installed())
    {
        return get_special_folder(common::local_app_data).combine(L"PicoTorrent");
    }

    TCHAR buf[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, buf);

    return buf;
}

fs::path environment::get_special_folder(picotorrent::common::special_folder folder)
{
    PWSTR buf;
    GUID g;

    switch (folder)
    {
    case picotorrent::common::user_downloads:
        g = FOLDERID_Downloads;
        break;
    case picotorrent::common::local_app_data:
        g = FOLDERID_LocalAppData;
        break;
    }

    HRESULT hr = SHGetKnownFolderPath(
        g,
        0,
        NULL,
        &buf);

    fs::path p(buf);
    CoTaskMemFree(buf);

    return p;
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
