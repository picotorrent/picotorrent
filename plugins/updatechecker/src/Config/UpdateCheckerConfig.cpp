#include "UpdateCheckerConfig.hpp"

using Config::UpdateCheckerConfig;

UpdateCheckerConfig::UpdateCheckerConfig(std::shared_ptr<picojson::object> config)
    : m_config(config)
{
}

std::string UpdateCheckerConfig::GetIgnoredVersion()
{
    if (m_config->find("ignored_version") == m_config->end())
    {
        return "";
    }

    return m_config->at("ignored_version").get<std::string>();
}

void UpdateCheckerConfig::SetIgnoredVersion(std::string const& version)
{
    (*m_config)["ignored_version"] = picojson::value(version);
}

std::string UpdateCheckerConfig::GetUpdateUrl()
{
    if (m_config->find("update_url") == m_config->end())
    {
        return "https://api.github.com/repos/picotorrent/picotorrent/releases/latest";
    }

    return m_config->at("update_url").get<std::string>();
}

