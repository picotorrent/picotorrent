#pragma once

#include <filesystem>
#include <string>

struct MMDB_s;

namespace pt
{
    class MaxMindDatabase
    {
    public:
        MaxMindDatabase();
        ~MaxMindDatabase();

        void load(std::filesystem::path const& databaseFile);
        std::string lookup(std::string const& ip);

    private:
        MMDB_s* m_db;
    };
}
