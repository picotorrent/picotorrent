#include "configuration.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "database.hpp"
#include "environment.hpp"

#include <Windows.h>

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

std::vector<Configuration::Filter> Configuration::GetFilters()
{
    std::vector<Filter> result;

    auto stmt = m_db->CreateStatement("select id, name, filter from filter");

    while (stmt->Read())
    {
        Filter f;
        f.id = stmt->GetInt(0);
        f.name= stmt->GetString(1);
        f.filter = stmt->GetString(2);

        result.push_back(f);
    }

    return result;
}

std::optional<Configuration::Filter> Configuration::GetFilterById(int id)
{
    auto stmt = m_db->CreateStatement("select id, name, filter from filter where id = $1");
    stmt->Bind(1, id);

    if (stmt->Execute())
    {
        Filter f;
        f.id = stmt->GetInt(0);
        f.name = stmt->GetString(1);
        f.filter = stmt->GetString(2);
        return f;
    }

    return std::nullopt;
}

std::vector<Configuration::Label> Configuration::GetLabels()
{
    std::vector<Label> result;

    auto stmt = m_db->CreateStatement("select id, name, color, color_enabled, save_path, save_path_enabled, apply_filter, apply_filter_enabled from label");

    while (stmt->Read())
    {
        Label lbl;
        lbl.id = stmt->GetInt(0);
        lbl.name = stmt->GetString(1);
        lbl.color = stmt->GetString(2);
        lbl.colorEnabled = stmt->GetBool(3);
        lbl.savePath = stmt->GetString(4);
        lbl.savePathEnabled = stmt->GetBool(5);
        lbl.applyFilter = stmt->GetString(6);
        lbl.applyFilterEnabled = stmt->GetBool(7);

        result.push_back(lbl);
    }

    return result;
}

void Configuration::DeleteLabel(int32_t id)
{
    {
        auto stmt = m_db->CreateStatement("update torrent set label_id = NULL where label_id = $1");
        stmt->Bind(1, id);
        stmt->Execute();
    }
    {
        auto stmt = m_db->CreateStatement("delete from label where id = $1");
        stmt->Bind(1, id);
        stmt->Execute();
    }
}

void Configuration::UpsertLabel(Configuration::Label const& label)
{
    if (label.id < 0)
    {
        auto stmt = m_db->CreateStatement("insert into label (name, color, color_enabled, save_path, save_path_enabled, apply_filter, apply_filter_enabled) values ($1, $2, $3, $4, $5, $6, $7);");
        stmt->Bind(1, label.name);
        stmt->Bind(2, label.color);
        stmt->Bind(3, label.colorEnabled);
        stmt->Bind(4, label.savePath);
        stmt->Bind(5, label.savePathEnabled);
        stmt->Bind(6, label.applyFilter);
        stmt->Bind(7, label.applyFilterEnabled);
        stmt->Execute();
    }
    else
    {
        auto stmt = m_db->CreateStatement("update label set name = $1, color = $2, color_enabled = $3, save_path = $4, save_path_enabled = $5, apply_filter = $6, apply_filter_enabled = $7 where id = $8");
        stmt->Bind(1, label.name);
        stmt->Bind(2, label.color);
        stmt->Bind(3, label.colorEnabled);
        stmt->Bind(4, label.savePath);
        stmt->Bind(5, label.savePathEnabled);
        stmt->Bind(6, label.applyFilter);
        stmt->Bind(7, label.applyFilterEnabled);
        stmt->Bind(8, label.id);
        stmt->Execute();
    }
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

bool Configuration::IsSystemDarkMode()
{
    bool systemUsesDarkTheme = false;
    HKEY hKey = 0;
    DWORD dwValue = 1;
    DWORD dwBufSize = sizeof(dwValue);
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        RegQueryValueEx(hKey, TEXT("AppsUseLightTheme"), NULL, NULL, (LPBYTE)&dwValue, &dwBufSize);
        RegCloseKey(hKey);
    }
    if (dwValue == 0)
    {
        systemUsesDarkTheme = true;
    }
    return systemUsesDarkTheme;
}

bool Configuration::IsDarkMode()
{
    return Configuration::Get<std::string>("theme_id").value_or("system") == "light"
        ? false
        : Configuration::IsSystemDarkMode();
}