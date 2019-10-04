#include "maxminddatabase.hpp"

#include <maxminddb.h>

using pt::MaxMindDatabase;

MaxMindDatabase::MaxMindDatabase()
    : m_db(new MMDB_s())
{
}

MaxMindDatabase::~MaxMindDatabase()
{
    MMDB_close(m_db);
    delete m_db;
}

void MaxMindDatabase::load(std::filesystem::path const& databaseFile)
{
    MMDB_close(m_db);
    MMDB_open(databaseFile.string().c_str(), MMDB_MODE_MMAP, m_db);
}

std::string MaxMindDatabase::lookup(std::string const& ip)
{
    int gaiError;
    int mmdbError;

    MMDB_lookup_result_s result = MMDB_lookup_string(
        m_db,
        ip.c_str(),
        &gaiError,
        &mmdbError);

    if (gaiError != 0)
    {
        return std::string();
    }

    if (mmdbError != MMDB_SUCCESS)
    {
        return std::string();
    }

    if (result.found_entry)
    {
        MMDB_entry_data_s data;

        int status = MMDB_get_value(
            &result.entry,
            &data,
            "country", "iso_code",
            nullptr);

        if (status != MMDB_SUCCESS)
        {
            return std::string();
        }

        if (data.has_data)
        {
            return std::string(data.utf8_string, data.data_size);
        }
    }

    return std::string();
}
