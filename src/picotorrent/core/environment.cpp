#include "environment.hpp"

#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>

#include "loguru.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;
using pt::Environment;

Environment::Environment()
    : m_startupTime(std::chrono::system_clock::now())
{
}

std::shared_ptr<Environment> Environment::create()
{
    auto env = new Environment();

    fs::path appData = env->getApplicationDataPath();

    if (!fs::exists(appData))
    {
        std::error_code ec;
        fs::create_directories(appData, ec);

        if (ec)
        {
            return nullptr;
        }
    }

    fs::path logPath = appData / "logs";

    if (!fs::exists(logPath))
    {
        std::error_code ec;
        fs::create_directories(logPath, ec);

        if (ec)
        {
            return nullptr;
        }
    }

    // Set up logging. PicoTorrent will log to the file,
    // - PicoTorrent.YYYYMMDDHHmmss.log
    // which represents the start up time.

    fs::path logFilePath = env->getLogFilePath();

    std::string s = Utils::toStdString(logFilePath.wstring());
    loguru::add_file(s.c_str(), loguru::Truncate, loguru::Verbosity_INFO);

    LOG_F(INFO, "PicoTorrent starting up...");

    return std::shared_ptr<Environment>(env);
}

fs::path Environment::getApplicationDataPath()
{
    if (isInstalled() || isAppContainerProcess())
    {
        return fs::path(getKnownFolderPath(KnownFolder::LocalAppData)) / "PicoTorrent";
    }

    return getApplicationPath();
}

fs::path Environment::getApplicationPath()
{
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);

    return path;
}

fs::path Environment::getDatabaseFilePath()
{
    return getApplicationDataPath() / "PicoTorrent.sqlite";
}

fs::path Environment::getKnownFolderPath(Environment::KnownFolder knownFolder)
{
    KNOWNFOLDERID fid = { 0 };

    switch (knownFolder)
    {
    case KnownFolder::LocalAppData:
        fid = FOLDERID_LocalAppData;
        break;

    case KnownFolder::UserDownloads:
        fid = FOLDERID_Downloads;
        break;

    default:
        LOG_F(FATAL, "Unknown KnownFolder specified: %d", knownFolder);
        throw std::runtime_error("Unknown folder");
    }

    PWSTR result;
    HRESULT hr = SHGetKnownFolderPath(fid, 0, nullptr, &result);

    if (SUCCEEDED(hr))
    {
        fs::path p(result);
        CoTaskMemFree(result);
        return p;
    }

    LOG_F(FATAL, "Failed to get KnownFolder: %d", knownFolder);

    throw std::runtime_error("Could not get known folder path");
}

fs::path Environment::getLogFilePath()
{
    std::time_t tim = std::chrono::system_clock::to_time_t(m_startupTime);
    tm t;
    localtime_s(&t, &tim);

    char frmt[100] = { 0 };
    snprintf(frmt,
        ARRAYSIZE(frmt),
        "PicoTorrent.%d%02d%02d%02d%02d%02d.log",
        t.tm_year + 1900,
        t.tm_mon + 1,
        t.tm_mday,
        t.tm_hour,
        t.tm_min,
        t.tm_sec);

    return getApplicationDataPath() / "logs" / frmt;
}

bool Environment::isAppContainerProcess()
{
    TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, ARRAYSIZE(path));
    PathRemoveFileSpec(path);
    PathCombine(path, path, TEXT("appx.dummy"));
    DWORD dwAttr = GetFileAttributes(path);

    return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}

bool Environment::isInstalled()
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
