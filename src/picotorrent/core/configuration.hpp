#pragma once

#include <memory>
#include <string>
#include <vector>

namespace pt
{
namespace Core
{
    class Database;
    class Environment;

    class Configuration
    {
    public:
        struct DhtBootstrapNode
        {
            int32_t id;
            std::string hostname;
            int32_t port;
        };

        struct ListenInterface
        {
            int32_t id;
            std::string address;
            int32_t port;
        };

        enum ConnectionProxyType
        {
            None,
            SOCKS4,
            SOCKS5,
            SOCKS5_Password,
            HTTP,
            HTTP_Password
        };

        enum class WindowState
        {
            Normal = 0,
            Minimized = 1,
            Hidden = 2,
            Maximized = 3
        };

        Configuration(std::shared_ptr<Database> db);
        ~Configuration();

        bool GetBool(std::string const& key);
        int GetInt(std::string const& key);
        std::string GetString(std::string const& key);

        void SetBool(std::string const& key, bool value);
        void SetInt(std::string const& key, int value);
        void SetString(std::string const& key, std::string const& value);

        std::vector<DhtBootstrapNode> GetDhtBootstrapNodes();

        std::vector<ListenInterface> GetListenInterfaces();
        void DeleteListenInterface(int id);
        void UpsertListenInterface(ListenInterface const& iface);

    private:
        std::shared_ptr<Database> m_db;
    };
}
}
