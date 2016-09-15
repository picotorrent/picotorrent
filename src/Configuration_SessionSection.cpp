#include "Configuration.hpp"

#include <memory>

#include <picojson.hpp>

namespace pj = picojson;

int Configuration::SessionSection::GetActiveChecking()
{
    return Get("session", "active_checking", 1);
}

int Configuration::SessionSection::GetActiveDhtLimit()
{
    return Get("session", "active_dht_limit", 88);
}

int Configuration::SessionSection::GetActiveDownloads()
{
    return Get("session", "active_downloads", 3);
}

int Configuration::SessionSection::GetActiveLimit()
{
    return Get("session", "active_limit", 15);
}

int Configuration::SessionSection::GetActiveLoadedLimit()
{
    return Get("session", "active_loaded_limit", 100);
}

int Configuration::SessionSection::GetActiveLsdLimit()
{
    return Get("session", "active_lsd_limit", 60);
}

int Configuration::SessionSection::GetActiveSeeds()
{
    return Get("session", "active_seeds", 5);
}

int Configuration::SessionSection::GetActiveTrackerLimit()
{
    return Get("session", "active_tracker_limit", 1600);
}

int Configuration::SessionSection::GetDownloadRateLimit()
{
    return Get("session", "download_rate_limit", 0);
}

void Configuration::SessionSection::SetDownloadRateLimit(int limit)
{
    Set("session", "download_rate_limit", limit);
}

int Configuration::SessionSection::GetStopTrackerTimeout()
{
    return Get("session", "stop_tracker_timeout", 1);
}

int Configuration::SessionSection::GetUploadRateLimit()
{
    return Get("session", "upload_rate_limit", 0);
}

void Configuration::SessionSection::SetUploadRateLimit(int limit)
{
    Set("session", "upload_rate_limit", limit);
}
