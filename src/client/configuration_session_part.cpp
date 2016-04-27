#include <picotorrent/client/configuration.hpp>

#include <memory>

#include <picojson.hpp>

namespace pj = picojson;
using picotorrent::client::configuration;

int configuration::session_part::active_checking()
{
    return get_part_key_or_default("session", "active_checking", 1);
}

int configuration::session_part::active_dht_limit()
{
    return get_part_key_or_default("session", "active_dht_limit", 88);
}

int configuration::session_part::active_downloads()
{
    return get_part_key_or_default("session", "active_downloads", 3);
}

int configuration::session_part::active_limit()
{
    return get_part_key_or_default("session", "active_limit", 15);
}

int configuration::session_part::active_loaded_limit()
{
    return get_part_key_or_default("session", "active_loaded_limit", 100);
}

int configuration::session_part::active_lsd_limit()
{
    return get_part_key_or_default("session", "active_lsd_limit", 60);
}

int configuration::session_part::active_seeds()
{
    return get_part_key_or_default("session", "active_seeds", 5);
}

int configuration::session_part::active_tracker_limit()
{
    return get_part_key_or_default("session", "active_tracker_limit", 1600);
}

int configuration::session_part::download_rate_limit()
{
    const char* key = __func__;

    // Check old configuration and do some cleanups if found.
    if (cfg_->find(key) != cfg_->end())
    {
        download_rate_limit((int)cfg_->at(key).get<int64_t>());
        cfg_->erase(key);
    }

    return get_part_key_or_default("session", key, 0);
}

void configuration::session_part::download_rate_limit(int limit)
{
    set_part_key("session", "download_rate_limit", limit);
}

int configuration::session_part::stop_tracker_timeout()
{
    return get_part_key_or_default("session", "stop_tracker_timeout", 1);
}

int configuration::session_part::upload_rate_limit()
{
    const char* key = __func__;

    // Check old configuration and do some cleanups if found.
    if (cfg_->find(key) != cfg_->end())
    {
        upload_rate_limit((int)cfg_->at(key).get<int64_t>());
        cfg_->erase(key);
    }

    return get_part_key_or_default("session", key, 0);
}

void configuration::session_part::upload_rate_limit(int limit)
{
    set_part_key("session", "upload_rate_limit", limit);
}
