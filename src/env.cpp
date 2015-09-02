#include "env.h"

#include <windows.h>

using namespace pico;

bool Env::IsInstalled()
{
    HKEY hKey;
    LSTATUS status = RegOpenKeyEx(
        HKEY_LOCAL_MACHINE,
        L"Software\\PicoTorrent",
        0,
        KEY_READ,
        &hKey);

    if (status != ERROR_SUCCESS)
    {
        return false;
    }

    TCHAR buf[512];
    DWORD size = _ARRAYSIZE(buf);

    status = RegQueryValueEx(
        hKey,
        L"InstalledVersion",
        0,
        NULL,
        (LPBYTE)buf,
        &size);

    return status == ERROR_SUCCESS;
}
