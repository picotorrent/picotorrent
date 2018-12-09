#pragma once

#include <memory>
#include <string>

namespace pt
{
    class Database;

    class Configuration
    {
    public:
        Configuration(std::shared_ptr<Database> db);
        ~Configuration();

        bool getBool(std::string const& key);
        int getInt(std::string const& key);
        std::string getString(std::string const& key);

        void setBool(std::string const& key, bool value);
        void setInt(std::string const& key, int value);
        void setString(std::string const& key, std::string const& value);

    private:
        std::shared_ptr<Database> m_db;
    };
}
