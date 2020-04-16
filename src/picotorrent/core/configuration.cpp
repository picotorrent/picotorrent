#include "configuration.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "database.hpp"
#include "environment.hpp"
#include "../picojson.hpp"

namespace fs = std::filesystem;

using pt::Core::Configuration;

Configuration::Configuration(std::shared_ptr<pt::Core::Database> db)
    : m_db(db)
{
}

Configuration::~Configuration()
{
}

void Configuration::Migrate(std::shared_ptr<pt::Core::Environment> env, std::shared_ptr<pt::Core::Configuration> cfg)
{
    fs::path oldConfigFile = env->GetApplicationDataPath() / "PicoTorrent.json";

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
    { cfg->SetString("default_save_path", obj["default_save_path"].get<std::string>()); }

    if (obj.find("language_id") != obj.end())
    { cfg->SetInt("language_id", static_cast<int>(obj["language_id"].get<int64_t>())); }

    if (obj.find("move_completed_downloads") != obj.end())
    { cfg->SetBool("move_completed_downloads", obj["move_completed_downloads"].get<bool>()); }

    if (obj.find("move_completed_downloads_from_default_only") != obj.end())
    { cfg->SetBool("move_completed_downloads_from_default_only", obj["move_completed_downloads_from_default_only"].get<bool>()); }

    if (obj.find("move_completed_downloads_path") != obj.end())
    { cfg->SetString("move_completed_downloads_path", obj["move_completed_downloads_path"].get<std::string>()); }

    // Proxy tings
    if (obj.find("proxy_host") != obj.end())
    { cfg->SetString("proxy_host", obj["proxy_host"].get<std::string>()); }

    if (obj.find("proxy_hostnames") != obj.end())
    { cfg->SetBool("proxy_hostnames", obj["proxy_hostnames"].get<bool>()); }

    if (obj.find("proxy_password") != obj.end())
    { cfg->SetString("proxy_password", obj["proxy_password"].get<std::string>()); }

    if (obj.find("proxy_peers") != obj.end())
    { cfg->SetBool("proxy_peers", obj["proxy_peers"].get<bool>()); }

    if (obj.find("proxy_port") != obj.end())
    { cfg->SetInt("proxy_port", static_cast<int>(obj["proxy_port"].get<int64_t>())); }

    if (obj.find("proxy_trackers") != obj.end())
    { cfg->SetBool("proxy_trackers", obj["proxy_trackers"].get<bool>()); }

    if (obj.find("proxy_type") != obj.end())
    { cfg->SetInt("proxy_type", static_cast<int>(obj["proxy_type"].get<int64_t>())); }

    if (obj.find("proxy_username") != obj.end())
    { cfg->SetString("proxy_username", obj["proxy_username"].get<std::string>()); }

    // Session
    if (obj.find("session") != obj.end() && obj["session"].is<picojson::object>())
    {
        picojson::object& sess = obj["session"].get<picojson::object>();

        if (sess.find("active_downloads") != sess.end())
        { cfg->SetInt("active_downloads", static_cast<int>(sess["active_downloads"].get<int64_t>())); }

        if (sess.find("active_limit") != sess.end())
        { cfg->SetInt("active_limit", static_cast<int>(sess["active_limit"].get<int64_t>())); }

        if (sess.find("active_seeds") != sess.end())
        { cfg->SetInt("active_seeds", static_cast<int>(sess["active_seeds"].get<int64_t>())); }

        if (sess.find("download_rate_limit") != sess.end())
        { cfg->SetInt("download_rate_limit", static_cast<int>(sess["download_rate_limit"].get<int64_t>())); }

        if (sess.find("enable_dht") != sess.end())
        { cfg->SetBool("enable_dht", sess["enable_dht"].get<bool>()); }

        if (sess.find("enable_download_rate_limit") != sess.end())
        { cfg->SetBool("enable_download_rate_limit", sess["enable_download_rate_limit"].get<bool>()); }

        if (sess.find("enable_lsd") != sess.end())
        { cfg->SetBool("enable_lsd", sess["enable_lsd"].get<bool>()); }

        if (sess.find("enable_pex") != sess.end())
        { cfg->SetBool("enable_pex", sess["enable_pex"].get<bool>()); }

        if (sess.find("enable_upload_rate_limit") != sess.end())
        { cfg->SetBool("enable_upload_rate_limit", sess["enable_upload_rate_limit"].get<bool>()); }

        if (sess.find("require_incoming_encryption") != sess.end())
        { cfg->SetBool("require_incoming_encryption", sess["require_incoming_encryption"].get<bool>()); }

        if (sess.find("require_outgoing_encryption") != sess.end())
        { cfg->SetBool("require_outgoing_encryption", sess["require_outgoing_encryption"].get<bool>()); }

        if (sess.find("upload_rate_limit") != sess.end())
        { cfg->SetInt("upload_rate_limit", static_cast<int>(sess["upload_rate_limit"].get<int64_t>())); }
    }

    // TODO: listen interfaces

    std::error_code ec;
    fs::rename(oldConfigFile, env->GetApplicationDataPath() / "PicoTorrent.json.old", ec);

    if (ec)
    {
        // TODO: log
    }
}

bool Configuration::GetBool(std::string const& key)
{
    return GetInt(key) > 0;
}

int Configuration::GetInt(std::string const& key)
{
    auto stmt = m_db->CreateStatement("select int_value from setting where key = ?");
    stmt->Bind(1, key);
    stmt->Execute();

    return stmt->GetInt(0);
}

std::string Configuration::GetString(std::string const& key)
{
    auto stmt = m_db->CreateStatement("select string_value from setting where key = ?");
    stmt->Bind(1, key);
    stmt->Execute();

    return stmt->GetString(0);
}


void Configuration::SetBool(std::string const& key, bool value)
{
    auto stmt = m_db->CreateStatement("update setting set int_value = ? where key = ?");
    stmt->Bind(1, value ? 1 : 0);
    stmt->Bind(2, key);
    stmt->Execute();
}

void Configuration::SetInt(std::string const& key, int value)
{
    auto stmt = m_db->CreateStatement("update setting set int_value = ? where key = ?");
    stmt->Bind(1, value);
    stmt->Bind(2, key);
    stmt->Execute();
}

void Configuration::SetString(std::string const& key, std::string const& value)
{
    auto stmt = m_db->CreateStatement("update setting set string_value = ? where key = ?");
    stmt->Bind(1, value);
    stmt->Bind(2, key);
    stmt->Execute();
}
