#pragma once

#include <memory>

namespace libtorrent
{
    struct settings_pack;
}

namespace pt
{
    class Configuration;

    class SessionSettings
    {
    public:
        static libtorrent::settings_pack Get(std::shared_ptr<Configuration> cfg);
    };
}
