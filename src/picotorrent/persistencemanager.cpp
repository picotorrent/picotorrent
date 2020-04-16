#include "persistencemanager.hpp"

#include "core/database.hpp"
#include "picojson.hpp"

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
        picojson::value v;
        std::string err = picojson::parse(v, stmt->GetString(0));

        if (err.empty())
        {
            *value = v.get<bool>();
            return true;
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
        picojson::value v;
        std::string err = picojson::parse(v, stmt->GetString(0));

        if (err.empty())
        {
            *value = static_cast<int>(v.get<int64_t>());
            return true;
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
        picojson::value v;
        std::string err = picojson::parse(v, stmt->GetString(0));

        if (err.empty())
        {
            *value = static_cast<long>(v.get<int64_t>());
            return true;
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
        picojson::value v;
        std::string err = picojson::parse(v, stmt->GetString(0));

        if (err.empty())
        {
            *value = wxString(v.get<std::string>());
            return true;
        }
    }

    return false;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, bool value)
{
    SaveValue(
        GetKey(who, name),
        picojson::value(value).serialize());

    return true;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, int value)
{
    SaveValue(
        GetKey(who, name),
        picojson::value(static_cast<int64_t>(value)).serialize());

    return true;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, long value)
{
    SaveValue(
        GetKey(who, name),
        picojson::value(static_cast<int64_t>(value)).serialize());

    return true;
}

bool PersistenceManager::SaveValue(const wxPersistentObject& who, const wxString& name, wxString value)
{
    SaveValue(
        GetKey(who, name),
        picojson::value(value).serialize());

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
