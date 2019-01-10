#include "maxminddatabase.hpp"

#include <maxminddb.h>

using pt::MaxMindDatabase;

MaxMindDatabase::MaxMindDatabase(std::string const& databaseFile)
{
    MMDB_open(databaseFile.c_str(), MMDB_MODE_MMAP, m_db);
}

MaxMindDatabase::~MaxMindDatabase()
{
    MMDB_close(m_db);
}

std::string MaxMindDatabase::lookup(std::string const& ip)
{
    int gaiError;
    int mmdbError;
    MMDB_lookup_result_s result = MMDB_lookup_string(m_db, ip.c_str(), &gaiError, &mmdbError);

    if (gaiError != 0)
    {
        return std::string();
    }

    if (mmdbError != MMDB_SUCCESS)
    {
        return std::string();
    }

    return std::string();
}
