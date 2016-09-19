#pragma once

#include <memory>
#include <string>

#include <picojson.hpp>

namespace Config
{
    struct UpdateCheckerConfig
    {
        UpdateCheckerConfig(std::shared_ptr<picojson::object> config);

        std::string GetIgnoredVersion();
        void SetIgnoredVersion(std::string const& version);
        std::string GetUpdateUrl();

    private:
        std::shared_ptr<picojson::object> m_config;
    };
}
