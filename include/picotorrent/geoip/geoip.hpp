#pragma once

#include <memory>
#include <string>

#include <picotorrent/export.hpp>

namespace pt
{
    class Environment;

    class GeoIP
    {
    public:
        PICO_API GeoIP(std::shared_ptr<Environment> env);

        bool PICO_API isAvailable();
        void PICO_API load();
        std::string PICO_API lookupCountryCode(std::string const& ip);

    private:
        std::shared_ptr<Environment> m_env;
    };
}
