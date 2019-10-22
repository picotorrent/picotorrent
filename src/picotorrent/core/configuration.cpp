#include "configuration.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "database.hpp"
#include "environment.hpp"
#include "../picojson.hpp"

namespace fs = std::filesystem;

using pt::Configuration;

Configuration::Configuration(std::shared_ptr<pt::Database> db)
    : m_db(db)
{
}

Configuration::~Configuration()
{
}

void Configuration::migrate(std::shared_ptr<pt::Environment> env, std::shared_ptr<pt::Configuration> cfg)
{
    fs::path oldConfigFile = env->getApplicationDataPath() / "PicoTorrent.json";

    if (!fs::exists(oldConfigFile))
    {
        return;
    }

    std::string contents;

    {
        // Separate scope to properly close the stream
        std::ifstream cfgStream(oldConfigFile, std::ios::binary);
        std::stringstream ss;
        ss << cfgStream.rdbuf();
        contents = ss.str();
    }

    picojson::value val;
    std::string error = picojson::parse(val, contents);

    if (!error.empty() || !val.is<picojson::object>())
    {
        return;
    }

    picojson::object obj = val.get<picojson::object>();

    // Migrate old settings

    if (obj.find("default_save_path") != obj.end())
    { cfg->setString("default_save_path", obj["default_save_path"].get<std::string>()); }

    if (obj.find("language_id") != obj.end())
    { cfg->setInt("language_id", static_cast<int>(obj["language_id"].get<int64_t>())); }

    if (obj.find("move_completed_downloads") != obj.end())
    { cfg->setBool("move_completed_downloads", obj["move_completed_downloads"].get<bool>()); }

    if (obj.find("move_completed_downloads_from_default_only") != obj.end())
    { cfg->setBool("move_completed_downloads_from_default_only", obj["move_completed_downloads_from_default_only"].get<bool>()); }

    if (obj.find("move_completed_downloads_path") != obj.end())
    { cfg->setString("move_completed_downloads_path", obj["move_completed_downloads_path"].get<std::string>()); }

    // Proxy tings
    if (obj.find("proxy_host") != obj.end())
    { cfg->setString("proxy_host", obj["proxy_host"].get<std::string>()); }

    if (obj.find("proxy_hostnames") != obj.end())
    { cfg->setBool("proxy_hostnames", obj["proxy_hostnames"].get<bool>()); }

    if (obj.find("proxy_password") != obj.end())
    { cfg->setString("proxy_password", obj["proxy_password"].get<std::string>()); }

    if (obj.find("proxy_peers") != obj.end())
    { cfg->setBool("proxy_peers", obj["proxy_peers"].get<bool>()); }

    if (obj.find("proxy_port") != obj.end())
    { cfg->setInt("proxy_port", static_cast<int>(obj["proxy_port"].get<int64_t>())); }

    if (obj.find("proxy_trackers") != obj.end())
    { cfg->setBool("proxy_trackers", obj["proxy_trackers"].get<bool>()); }

    if (obj.find("proxy_type") != obj.end())
    { cfg->setInt("proxy_type", static_cast<int>(obj["proxy_type"].get<int64_t>())); }

    if (obj.find("proxy_username") != obj.end())
    { cfg->setString("proxy_username", obj["proxy_username"].get<std::string>()); }

    // Session
    if (obj.find("session") != obj.end() && obj["session"].is<picojson::object>())
    {
        picojson::object& sess = obj["session"].get<picojson::object>();

        if (sess.find("active_downloads") != sess.end())
        { cfg->setInt("active_downloads", static_cast<int>(sess["active_downloads"].get<int64_t>())); }

        if (sess.find("active_limit") != sess.end())
        { cfg->setInt("active_limit", static_cast<int>(sess["active_limit"].get<int64_t>())); }

        if (sess.find("active_seeds") != sess.end())
        { cfg->setInt("active_seeds", static_cast<int>(sess["active_seeds"].get<int64_t>())); }

        if (sess.find("download_rate_limit") != sess.end())
        { cfg->setInt("download_rate_limit", static_cast<int>(sess["download_rate_limit"].get<int64_t>())); }

        if (sess.find("enable_dht") != sess.end())
        { cfg->setBool("enable_dht", sess["enable_dht"].get<bool>()); }

        if (sess.find("enable_download_rate_limit") != sess.end())
        { cfg->setBool("enable_download_rate_limit", sess["enable_download_rate_limit"].get<bool>()); }

        if (sess.find("enable_lsd") != sess.end())
        { cfg->setBool("enable_lsd", sess["enable_lsd"].get<bool>()); }

        if (sess.find("enable_pex") != sess.end())
        { cfg->setBool("enable_pex", sess["enable_pex"].get<bool>()); }

        if (sess.find("enable_upload_rate_limit") != sess.end())
        { cfg->setBool("enable_upload_rate_limit", sess["enable_upload_rate_limit"].get<bool>()); }

        if (sess.find("require_incoming_encryption") != sess.end())
        { cfg->setBool("require_incoming_encryption", sess["require_incoming_encryption"].get<bool>()); }

        if (sess.find("require_outgoing_encryption") != sess.end())
        { cfg->setBool("require_outgoing_encryption", sess["require_outgoing_encryption"].get<bool>()); }

        if (sess.find("upload_rate_limit") != sess.end())
        { cfg->setInt("upload_rate_limit", static_cast<int>(sess["upload_rate_limit"].get<int64_t>())); }
    }

    // TODO: listen interfaces

    std::error_code ec;
    fs::rename(oldConfigFile, env->getApplicationDataPath() / "PicoTorrent.json.old", ec);

    if (ec)
    {
        // TODO: log
    }
}

bool Configuration::getBool(std::string const& key)
{
    return getInt(key) > 0;
}

int Configuration::getInt(std::string const& key)
{
    auto stmt = m_db->statement("select int_value from setting where key = ?");
    stmt->bind(1, key);
    stmt->execute();

    return stmt->getInt(0);
}

std::string Configuration::getString(std::string const& key)
{
    auto stmt = m_db->statement("select string_value from setting where key = ?");
    stmt->bind(1, key);
    stmt->execute();

    return stmt->getString(0);
}


void Configuration::setBool(std::string const& key, bool value)
{
    auto stmt = m_db->statement("update setting set int_value = ? where key = ?");
    stmt->bind(1, value ? 1 : 0);
    stmt->bind(2, key);
    stmt->execute();
}

void Configuration::setInt(std::string const& key, int value)
{
    auto stmt = m_db->statement("update setting set int_value = ? where key = ?");
    stmt->bind(1, value);
    stmt->bind(2, key);
    stmt->execute();
}

void Configuration::setString(std::string const& key, std::string const& value)
{
    auto stmt = m_db->statement("update setting set string_value = ? where key = ?");
    stmt->bind(1, value);
    stmt->bind(2, key);
    stmt->execute();
}
