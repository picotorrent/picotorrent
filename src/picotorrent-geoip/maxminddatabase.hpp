#pragma once

#include <string>

struct MMDB_s;

namespace pt
{
    class MaxMindDatabase
    {
    public:
        MaxMindDatabase(std::string const& databaseFile);
        ~MaxMindDatabase();

        std::string lookup(std::string const& ip);

    private:
        MMDB_s* m_db;
    };
}
