#include "config.hpp"

#include <memory>

#include "picojson.hpp"

namespace pj = picojson;
using pt::Configuration;

int Configuration::SessionSection::ActiveChecking()
{
    return Get("session", "active_checking", 1);
}

int Configuration::SessionSection::ActiveDhtLimit()
{
    return Get("session", "active_dht_limit", 88);
}

int Configuration::SessionSection::ActiveDownloads()
{
    return Get("session", "active_downloads", 3);
}

void Configuration::SessionSection::ActiveDownloads(int value)
{
    Set("session", "active_downloads", value);
}

int Configuration::SessionSection::ActiveLimit()
{
    return Get("session", "active_limit", 15);
}

void Configuration::SessionSection::ActiveLimit(int value)
{
    Set("session", "active_limit", value);
}

int Configuration::SessionSection::ActiveLsdLimit()
{
    return Get("session", "active_lsd_limit", 60);
}

int Configuration::SessionSection::ActiveSeeds()
{
    return Get("session", "active_seeds", 5);
}

void Configuration::SessionSection::ActiveSeeds(int value)
{
    Set("session", "active_seeds", value);
}

int Configuration::SessionSection::ActiveTrackerLimit()
{
    return Get("session", "active_tracker_limit", 1600);
}

bool Configuration::SessionSection::EnableAnonymousMode()
{
    return Get("session", "anonymous_mode", false);
}

bool Configuration::SessionSection::EnableDht()
{
    return Get("session", "enable_dht", true);
}

void Configuration::SessionSection::EnableDht(bool value)
{
    Set("session", "enable_dht", value);
}

bool Configuration::SessionSection::EnableLsd()
{
    return Get("session", "enable_lsd", true);
}

void Configuration::SessionSection::EnableLsd(bool value)
{
    Set("session", "enable_lsd", value);
}

bool Configuration::SessionSection::EnableDownloadRateLimit()
{
    return Get("session", "enable_download_rate_limit", false);
}

void Configuration::SessionSection::EnableDownloadRateLimit(bool value)
{
    Set("session", "enable_download_rate_limit", value);
}

bool Configuration::SessionSection::EnableUploadRateLimit()
{
    return Get("session", "enable_upload_rate_limit", false);
}

void Configuration::SessionSection::EnableUploadRateLimit(bool value)
{
    Set("session", "enable_upload_rate_limit", value);
}

int Configuration::SessionSection::DownloadRateLimit()
{
    return Get("session", "download_rate_limit", 1024);
}

void Configuration::SessionSection::DownloadRateLimit(int limit)
{
    Set("session", "download_rate_limit", limit);
}

bool Configuration::SessionSection::RequireIncomingEncryption()
{
    return Get("session", "require_incoming_encryption", false);
}

void Configuration::SessionSection::RequireIncomingEncryption(bool value)
{
    Set("session", "require_incoming_encryption", value);
}

bool Configuration::SessionSection::RequireOutgoingEncryption()
{
    return Get("session", "require_outgoing_encryption", false);
}

void Configuration::SessionSection::RequireOutgoingEncryption(bool value)
{
    Set("session", "require_outgoing_encryption", value);
}

int Configuration::SessionSection::StopTrackerTimeout()
{
    return Get("session", "stop_tracker_timeout", 1);
}

int Configuration::SessionSection::UploadRateLimit()
{
    return Get("session", "upload_rate_limit", 1024);
}

void Configuration::SessionSection::UploadRateLimit(int limit)
{
    Set("session", "upload_rate_limit", limit);
}
