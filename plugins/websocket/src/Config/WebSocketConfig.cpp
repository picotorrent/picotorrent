#include "WebSocketConfig.hpp"

using Config::WebSocketConfig;

WebSocketConfig::WebSocketConfig(std::shared_ptr<picojson::object> config)
    : m_config(config)
{
}

bool WebSocketConfig::IsEnabled()
{
    if (m_config->find("websocket") != m_config->end()
        && m_config->at("websocket").is<picojson::object>())
    {
        picojson::object cfg = m_config->at("websocket").get<picojson::object>();

        if (cfg.find("enabled") != cfg.end()
            && cfg.at("enabled").is<bool>())
        {
            return cfg.at("enabled").get<bool>();
        }
    }

    return false;
}

int WebSocketConfig::ListenPort()
{
    if (m_config->find("websocket") != m_config->end()
        && m_config->at("websocket").is<picojson::object>())
    {
        picojson::object cfg = m_config->at("websocket").get<picojson::object>();

        if (cfg.find("listen_port") != cfg.end()
            && cfg.at("listen_port").is<int64_t>())
        {
            return static_cast<int>(cfg.at("listen_port").get<int64_t>());
        }
    }

    return 7676;
}
