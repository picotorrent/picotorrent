#pragma once

#include <memory>
#include <string>

namespace pt
{
    class Database;
    class Environment;

    class Configuration
    {
    public:
        enum ConnectionProxyType
        {
            None,
            SOCKS4,
            SOCKS5,
            SOCKS5_Password,
            HTTP,
            HTTP_Password
        };

        enum WindowState
        {
            Normal,
            Minimized,
            Hidden,
            Maximized
        };

        Configuration(std::shared_ptr<Database> db);
        ~Configuration();

        static void migrate(std::shared_ptr<Environment> env, std::shared_ptr<Configuration> cfg);

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
