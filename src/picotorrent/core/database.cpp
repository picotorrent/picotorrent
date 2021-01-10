#include "database.hpp"

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#endif

#include <filesystem>
#include <vector>

#include <boost/log/trivial.hpp>
#include <sqlite3.h>

#include "environment.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;
using pt::Core::Database;

struct Migration
{
    std::string name;
    std::string sql;
};
#ifdef _WIN32
static BOOL CALLBACK EnumMigrations(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    std::vector<Migration>* migrations = reinterpret_cast<std::vector<Migration>*>(lParam);

    HRSRC rc = FindResource(hModule, lpszName, lpszType);
    DWORD size = SizeofResource(hModule, rc);
    HGLOBAL data = LoadResource(hModule, rc);
    const char* buffer = reinterpret_cast<const char*>(LockResource(data));

    Migration m;
    m.name = pt::Utils::toStdString(lpszName);
    m.sql  = std::string(buffer, size);

    migrations->push_back(m);

    return TRUE;
}
#endif
Database::Statement::Statement(sqlite3_stmt* stmt)
    : m_stmt(stmt)
{
}

Database::Statement::~Statement()
{
    sqlite3_finalize(m_stmt);
}

void Database::Statement::Bind(int idx, int value)
{
    sqlite3_bind_int(m_stmt, idx, value);
}

void Database::Statement::Bind(int idx, std::optional<int> value)
{
    value.has_value()
        ? sqlite3_bind_int(m_stmt, idx, value.value())
        : sqlite3_bind_null(m_stmt, idx);
}

void Database::Statement::Bind(int idx, std::string const& value)
{
    sqlite3_bind_text(
        m_stmt,
        idx,
        value.c_str(),
        static_cast<int>(value.size()),
        SQLITE_TRANSIENT);
}

void Database::Statement::Bind(int idx, std::vector<char> const& value)
{
    int res = sqlite3_bind_blob(
        m_stmt,
        idx,
        reinterpret_cast<const void*>(value.data()),
        static_cast<int>(value.size()),
        SQLITE_TRANSIENT);

    if (res != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to bind argument: " << res;
    }
}

bool Database::Statement::Execute()
{
    int res = sqlite3_step(m_stmt);

    if (res != SQLITE_ROW && res != SQLITE_DONE)
    {
        const char* err = sqlite3_errmsg(sqlite3_db_handle(m_stmt));
        BOOST_LOG_TRIVIAL(error) << "Failed to execute statement: " << err;
        return false;
    }

    return true;
}

void Database::Statement::GetBlob(int idx, std::vector<char>& result)
{
    int len = sqlite3_column_bytes(m_stmt, idx);
    const char* buf = reinterpret_cast<const char*>(sqlite3_column_blob(m_stmt, idx));

    result.clear();
    result.insert(result.begin(), buf, buf + len);
}

bool Database::Statement::GetBool(int idx)
{
    return (sqlite3_column_int(m_stmt, idx) > 0);
}

int Database::Statement::GetInt(int idx)
{
    return sqlite3_column_int(m_stmt, idx);
}

std::string Database::Statement::GetString(int idx)
{
    const unsigned char* res = sqlite3_column_text(m_stmt, idx);

    if (res == nullptr)
    {
        return std::string();
    }

    return std::string(
        reinterpret_cast<const char*>(res),
        sqlite3_column_bytes(m_stmt, idx));
}

bool Database::Statement::Read()
{
    if (sqlite3_step(m_stmt) == SQLITE_ROW)
    {
        return true;
    }

    return false;
}

Database::Database(std::shared_ptr<pt::Core::Environment> env)
    : m_env(env)
{
    fs::path dbFile = env->GetDatabaseFilePath();
    std::string convertedPath = dbFile.c_str(); //::toStdString(dbFile.wstring());

    BOOST_LOG_TRIVIAL(info) << "Loading PicoTorrent database from " << convertedPath;

    sqlite3_open(convertedPath.c_str(), &m_db);

    Execute("PRAGMA foreign_keys = ON;");

    sqlite3_create_function(
        m_db,
        "get_known_folder_path",
        1,
        SQLITE_ANY,
        nullptr,
        GetKnownFolderPath,
        nullptr,
        nullptr);

    sqlite3_create_function(
        m_db,
        "get_user_default_ui_language",
        0,
        SQLITE_ANY,
        nullptr,
        GetUserDefaultUILanguage,
        nullptr,
        nullptr);
}

Database::~Database()
{
    sqlite3_close(m_db);
}

void Database::Execute(std::string const& sql)
{
    auto stmt = CreateStatement(sql);
    stmt->Execute();
}

bool Database::Migrate()
{
    // create migration_history table
    const char* migrationHistory = "create table if not exists migration_history ("
            "id integer primary key,"
            "name text not null unique"
        ");";

    Execute(migrationHistory);

    std::vector<Migration> migrations;

    sqlite3* cdb;
    sqlite3_open(m_env->GetCoreDbFilePath().c_str(), &cdb);
    sqlite3_exec(
        cdb,
        "SELECT id, content FROM migrations ORDER BY id ASC",
        [](void* user, int, char** data, char**)
        {
            std::vector<Migration>* m = static_cast<std::vector<Migration>*>(user);
            m->push_back({ data[0], data[1] });
            return 0;
        },
        &migrations,
        nullptr);

    //m_env->GetCoreDbFilePath()

#ifdef _WIN32
    EnumResourceNames(
        NULL,
        TEXT("DBMIGRATION"),
        &EnumMigrations,
        reinterpret_cast<LONG_PTR>(&migrations));
#endif

    BOOST_LOG_TRIVIAL(info) << "Found " << migrations.size() << " migrations";

    Execute("BEGIN TRANSACTION;");

    for (Migration const& m : migrations)
    {
        if (MigrationExists(m.name))
        {
            continue;
        }

        const char* sql = m.sql.c_str();
        sqlite3_stmt* statement = nullptr;

        // Use raw sqlite code here to support multiple statements in one migration.
        do {
            if (sqlite3_prepare_v2(
                m_db,
                sql,
                -1,
                &statement,
                &sql) != SQLITE_OK)
            {
                BOOST_LOG_TRIVIAL(error) << "Failed to prepare migration " << m.name << " (" << sqlite3_errmsg(m_db) << ")";
                return false;
            }

            if (statement == nullptr)
            {
                break;
            }

            int stepResult = sqlite3_step(statement);

            if (stepResult != SQLITE_OK && stepResult != SQLITE_DONE)
            {
                BOOST_LOG_TRIVIAL(error) << "Failed to step/execute migration " << m.name << " (" << sqlite3_errmsg(m_db) << ")";
                return false;
            }

            sqlite3_finalize(statement);
        } while (sql && sql[0] != '\0');

        auto stmt = CreateStatement("insert into migration_history (name) values (?);");
        stmt->Bind(1, m.name.c_str());
        stmt->Execute();

        BOOST_LOG_TRIVIAL(info) << "Migration " << m.name << " applied";
    }

    Execute("COMMIT;");

    return true;
}

std::shared_ptr<Database::Statement> Database::CreateStatement(std::string const& sql)
{
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_ERROR)
    {
        const char* err = sqlite3_errmsg(m_db);
        BOOST_LOG_TRIVIAL(error) << "failed to execute SQL statement: " << err;
        throw std::runtime_error(err);
    }

    return std::shared_ptr<Statement>(new Statement(stmt));
}

void Database::GetKnownFolderPath(sqlite3_context* ctx, int argc, sqlite3_value** argv)
{
#ifdef _WIN32
    if (argc > 0)
    {
        std::string folderId((const char*)sqlite3_value_text(argv[0]));
        KNOWNFOLDERID fid = { 0 };

        if (folderId == "FOLDERID_Downloads")
        {
            fid = FOLDERID_Downloads;
        }
        else
        {
            return;
        }

        PWSTR result;
        HRESULT hr = SHGetKnownFolderPath(fid, 0, nullptr, &result);

        if (SUCCEEDED(hr))
        {
            std::string res = Utils::toStdString(result);
            sqlite3_result_text(ctx, res.c_str(), -1, SQLITE_TRANSIENT);
        }
    }
#endif
}

void Database::GetUserDefaultUILanguage(sqlite3_context* ctx, int, sqlite3_value**)
{
#ifdef _WIN32
    sqlite3_result_int(ctx, static_cast<int>(::GetUserDefaultUILanguage()));
#endif
}

bool Database::MigrationExists(std::string const& name)
{
    auto stmt = CreateStatement("select count(*) from migration_history where name = ?");
    stmt->Bind(1, name.c_str());
    stmt->Execute();
    return stmt->GetInt(0) > 0;
}
