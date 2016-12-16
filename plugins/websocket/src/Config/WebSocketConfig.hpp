#pragma once

#include <memory>
#include <string>

#include <picojson.hpp>

namespace Config
{
    struct WebSocketConfig
    {
        WebSocketConfig(std::shared_ptr<picojson::object> config);

        bool IsEnabled();
        int ListenPort();

    private:
        std::shared_ptr<picojson::object> m_config;
    };
}
