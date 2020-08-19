#include "persistencemanager.hpp"

#include <loguru.hpp>
#include <nlohmann/json.hpp>

#include "core/database.hpp"

using json = nlohmann::json;
using pt::PersistenceManager;

PersistenceManager::PersistenceManager(std::shared_ptr<Core::Database> db)
    : m_db(db)
{
}

PersistenceManager::~PersistenceManager()
{
}

bool PersistenceManager::RestoreValue(const wxPersistentObject& who, const wxString& name, bool* value)
{
    auto stmt = m_db->CreateStatement("SELECT value FROM persistent_object WHERE key = ?");
    stmt->Bind(1, GetKey(who, name).ToStdString());
    
    if (stmt->Read())
    {
        try
        {
            json j = json::parse(stmt->GetString(0));
            *value = j.get<bool>();
            return true;
        }
        catch (std::exception const& ex)
        {
            LOG_F(ERROR, "Failed to parse JSON as bool: %s", ex.what());
            return false;
        }
    }

    return false;
}

bool PersistenceManager::RestoreValue(const wxPersistentObject& who, const wxString& name, int* value)
{
    auto stmt = m_db->CreateStatement("SELECT value FROM persistent_object WHERE key = ?");
    stmt->Bind(1, GetKey(who, name).ToStdString());

    if (stmt->Read())
    {
        try
        {
            json j = json::parse(stmt->GetString(0));
            *value = j.get<int>();
            return true;
        }
        catch (std::exception const& ex)
        {
            LOG_F(ERROR, "Failed to parse JSON as int: %s", ex.what());
            return false;
        }
    }

    return false;
}

bool PersistenceManager::RestoreValue(const wxPersistentObject& who, const wxString& name, long* value)
{
    auto stmt = m_db->CreateStatement("SELECT value FROM persistent_object WHERE key = ?");
    stmt->Bind(1, GetKey(who, name).ToStdString());

    if (stmt->Read())
    {
        try
        {
            json j = json::parse(stmt->GetString(0));
            *value = j.get<long>();
            return true;
        }
        catch (std::exception const& ex)
        {
            LOG_F(ERROR, "Failed to parse JSON as long: %s", ex.what());
            return false;
        }
    }

    return false;
}

bool PersistenceManager::RestoreValue(const wxPersistentObject& who, const wxString& name, wxString* value)
{
    auto stmt = m_db->CreateStatement("SELECT value FROM persistent_object WHERE key = ?");
    stmt->Bind(1, GetKey(who, name).ToStdString());

    if (stmt->Read())
    {
        try
        {
            json j = json::parse(stmt->GetString(0));
            *value = j.get<std::string>();
            return true;
        }
        catch (std::exception const& ex)
        {
            LOG_F(ERROR, "Failed to parse JSON as string: %s", ex.what());
            return false;
        }
    }

    return false;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, bool value)
{
    SaveValue(
        GetKey(who, name),
        json(value).dump());

    return true;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, int value)
{
    SaveValue(
        GetKey(who, name),
        json(value).dump());

    return true;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, long value)
{
    SaveValue(
        GetKey(who, name),
        json(value).dump());

    return true;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, wxString value)
{
    SaveValue(
        GetKey(who, name),
        json(value.ToStdString()).dump());

    return true;
}

wxString PersistenceManager::GetKey(const wxPersistentObject& who, const wxString& name)
{
    return who.GetKind()
        << wxCONFIG_PATH_SEPARATOR << who.GetName()
        << wxCONFIG_PATH_SEPARATOR << name;
}

void PersistenceManager::SaveValue(const wxString& key, std::string const& value)
{
    auto stmt = m_db->CreateStatement("INSERT OR REPLACE INTO persistent_object(key, value) VALUES(?,?);");
    stmt->Bind(1, key.ToStdString());
    stmt->Bind(2, value);
    stmt->Execute();
}
