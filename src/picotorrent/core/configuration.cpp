#include "configuration.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "database.hpp"
#include "environment.hpp"

namespace fs = std::filesystem;

using pt::Core::Configuration;

Configuration::Configuration(std::shared_ptr<pt::Core::Database> db)
    : m_db(db)
{
}

Configuration::~Configuration()
{
}

bool Configuration::GetValue(std::string const& key, std::string& val)
{
    auto stmt = m_db->CreateStatement("SELECT IFNULL(value, default_value) FROM setting WHERE key = ?");
    stmt->Bind(1, key);
    
    if (!stmt->Execute())
    {
        return false;
    }

    val = stmt->GetString(0);

    return true;
}

void Configuration::SetValue(std::string const& key, std::string const& val)
{
    auto stmt = m_db->CreateStatement("UPDATE setting SET value = ? WHERE key = ?");
    stmt->Bind(1, val);
    stmt->Bind(2, key);
    stmt->Execute();
}

void Configuration::RestoreDefaults()
{
    m_db->Execute("UPDATE setting SET value = (SELECT default_value FROM setting s2 WHERE s2.key = setting.key);");
}

std::vector<Configuration::DhtBootstrapNode> Configuration::GetDhtBootstrapNodes()
{
    std::vector<DhtBootstrapNode> result;

    auto stmt = m_db->CreateStatement("select id, hostname, port from dht_bootstrap_node");

    while (stmt->Read())
    {
        DhtBootstrapNode node;
        node.id = stmt->GetInt(0);
        node.hostname = stmt->GetString(1);
        node.port = stmt->GetInt(2);

        result.push_back(node);
    }

    return result;
}

std::vector<Configuration::ListenInterface> Configuration::GetListenInterfaces()
{
    std::vector<ListenInterface> result;

    auto stmt = m_db->CreateStatement("select id, address, port from listen_interface");

    while (stmt->Read())
    {
        ListenInterface li;
        li.id = stmt->GetInt(0);
        li.address = stmt->GetString(1);
        li.port = stmt->GetInt(2);

        result.push_back(li);
    }

    return result;
}

void Configuration::DeleteListenInterface(int id)
{
    auto stmt = m_db->CreateStatement("delete from listen_interface where id = ?");
    stmt->Bind(1, id);
    stmt->Execute();
}

void Configuration::UpsertListenInterface(Configuration::ListenInterface const& iface)
{
    if (iface.id < 0)
    {
        auto stmt = m_db->CreateStatement("insert into listen_interface (address, port) values (?, ?);");
        stmt->Bind(1, iface.address);
        stmt->Bind(2, iface.port);
        stmt->Execute();
    }
    else
    {
        auto stmt = m_db->CreateStatement("update listen_interface set address = ?, port = ? where id = ?");
        stmt->Bind(1, iface.address);
        stmt->Bind(2, iface.port);
        stmt->Bind(3, iface.id);
        stmt->Execute();
    }
}
