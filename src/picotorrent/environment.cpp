#include "environment.hpp"

#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>

using pt::Environment;

fs::path Environment::GetApplicationDataPath()
{
    if (IsInstalled() || IsAppContainerProcess())
    {
        return fs::path(GetKnownFolderPath(KnownFolder::LocalAppData)) / "PicoTorrent";
    }

    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);

    return path;
}

fs::path Environment::GetKnownFolderPath(Environment::KnownFolder knownFolder)
{
    KNOWNFOLDERID rfid;

    switch (knownFolder)
    {
    case KnownFolder::LocalAppData:
        rfid = FOLDERID_LocalAppData;
        break;
    case KnownFolder::UserDownloads:
        rfid = FOLDERID_Downloads;
        break;
    default:
        throw std::runtime_error("Unknown folder");
    }

    PWSTR buf;
    HRESULT hResult = SHGetKnownFolderPath(
        rfid,
        0,
        NULL,
        &buf);

    if (hResult != S_OK)
    {
        throw std::runtime_error(std::to_string(hResult).c_str());
    }

    std::wstring res = buf;
    CoTaskMemFree(buf);

    return res;
}

bool Environment::IsAppContainerProcess()
{
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);
    PathCombine(path, path, TEXT("appx.dummy"));
    DWORD dwAttr = GetFileAttributes(path);

    return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
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
