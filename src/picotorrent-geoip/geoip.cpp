#include <picotorrent/geoip/geoip.hpp>

#include <fstream>
#include <iterator>
#include <vector>

#include <maxminddb.h>
#include <picotorrent/core/environment.hpp>

#include "gzipdecompressor.hpp"

using pt::GeoIP;

GeoIP::GeoIP(std::shared_ptr<pt::Environment> env)
    : m_env(env)
{
}

std::string GeoIP::lookupCountryCode(std::string const& ip)
{
    MMDB_s* db;
    MMDB_open("he", 12, db);

    return std::string();
}

bool GeoIP::isAvailable()
{
    return false;
}

void GeoIP::load()
{
}
