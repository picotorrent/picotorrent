#include "Environment.hpp"

#include <windows.h>
#include <shlwapi.h>

std::wstring Environment::GetApplicationPath()
{
    TCHAR buf[MAX_PATH];
    GetModuleFileName(NULL, buf, ARRAYSIZE(buf));
    PathRemoveFileSpec(buf);
    return buf;
}

std::wstring Environment::GetDataPath()
{
    if (IsInstalled())
    {
        return GetKnownFolderPath(FOLDERID_LocalAppData);
    }

    return GetApplicationPath();
}

std::wstring Environment::GetKnownFolderPath(const KNOWNFOLDERID& rfid)
{
    PWSTR buf;
    HRESULT hResult = SHGetKnownFolderPath(
        rfid,
        0,
        NULL,
        &buf);

    std::wstring res = buf;
    CoTaskMemFree(buf);

    return res;
}


bool Environment::IsInstalled()
{
    HKEY hKey = NULL;

    LSTATUS lStatus = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        TEXT("Software\\PicoTorrent"),
        0,
        KEY_READ,
        &hKey);

    if (lStatus != ERROR_SUCCESS)
    {
        if (hKey != NULL) { RegCloseKey(hKey); }
        return false;
    }

    TCHAR installDirectory[MAX_PATH];
    DWORD bufSize = MAX_PATH;

    lStatus = RegQueryValueEx(
        hKey,
        L"InstallDirectory",
        NULL,
        NULL,
        (LPBYTE)installDirectory,
        &bufSize);

    if (lStatus != ERROR_SUCCESS)
    {
        if (hKey != NULL) { RegCloseKey(hKey); }
        return false;
    }

    TCHAR currentLocation[MAX_PATH];
    GetModuleFileName(NULL, currentLocation, ARRAYSIZE(currentLocation));

    TCHAR installedFile[MAX_PATH];
    PathCombine(installedFile, installDirectory, TEXT("PicoTorrent.exe"));

    if (StrCmp(currentLocation, installedFile) == 0)
    {
        if (hKey != NULL) { RegCloseKey(hKey); }
        return true;
    }

    if (hKey != NULL) { RegCloseKey(hKey); }
    return false;
}
