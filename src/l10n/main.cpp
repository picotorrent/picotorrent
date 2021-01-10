#include <fstream>
#include <filesystem>
#include <iostream>

#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace fs = std::filesystem;
using nlohmann::json;

const char* translations_table_sql = "CREATE TABLE translations (id INTEGER PRIMARY KEY, locale TEXT NOT NULL, key TEXT NOT NULL, value TEXT, UNIQUE(locale, key));";
const char* insert_translation_sql = "INSERT INTO translations (locale, key, value) VALUES (?, ?, ?);";

const char* migrations_table_sql = "CREATE TABLE migrations (id TEXT PRIMARY KEY, content TEXT NOT NULL)";
const char * insert_migration_sql = "INSERT INTO migrations (id, content) VALUES (?, ?);";

bool insert_migrations(fs::path const& path, sqlite3* db)
{
    if (sqlite3_exec(db, migrations_table_sql, nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "could not create migrations table" << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, insert_migration_sql, -1, &stmt, nullptr);

    for (fs::path p : fs::directory_iterator(path))
    {
        std::string migration = p.filename().string();
        auto ext_pos = migration.find_last_of('.');

        if (ext_pos != std::string::npos)
        {
            migration = migration.substr(0, ext_pos);
        }

        std::cout << "inserting migration " << migration << std::endl;

        std::ifstream in(p);
        std::string content(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>());

        sqlite3_bind_text(stmt, 1, migration.c_str(), static_cast<int>(migration.size()), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, content.c_str(),   static_cast<int>(content.size()),   SQLITE_TRANSIENT);

        int res = sqlite3_step(stmt);

        if (res != SQLITE_ROW && res != SQLITE_DONE)
        {
            std::cerr << "error when inserting migration data: " << sqlite3_errmsg(sqlite3_db_handle(stmt)) << std::endl;
            return false;
        }

        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);

    return true;
}

bool insert_translations(fs::path const& path, sqlite3* db)
{
    if (sqlite3_exec(db, translations_table_sql, nullptr, nullptr, nullptr) != SQLITE_OK)
    {
        std::cerr << "could not create translations table" << std::endl;
        return false;
    }

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, insert_translation_sql, -1, &stmt, nullptr);

    for (fs::path p : fs::directory_iterator(path))
    {
        std::string loc = p.filename().string();
        auto ext_pos = loc.find_last_of('.');

        if (ext_pos != std::string::npos)
        {
            loc = loc.substr(0, ext_pos);
        }

        std::ifstream in(p);
        json j;
        in >> j;

        std::cout << "inserting " << j.size() << " items for " << loc << std::endl;

        for (auto const& line : j.items())
        {
            std::string const& key = line.key();
            std::string const& val = line.value().get<std::string>();

            sqlite3_bind_text(stmt, 1, loc.c_str(), static_cast<int>(loc.size()), SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, key.c_str(), static_cast<int>(key.size()), SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, val.c_str(), static_cast<int>(val.size()), SQLITE_TRANSIENT);

            int res = sqlite3_step(stmt);

            if (res != SQLITE_ROW && res != SQLITE_DONE)
            {
                std::cerr << "error when inserting translation data: " << sqlite3_errmsg(sqlite3_db_handle(stmt)) << std::endl;
                std::cerr << " - " << loc << std::endl << " - " << key << std::endl;
                return false;
            }

            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);

    return true;
}

int main(int argc, char* argv[])
{
    std::cout << "generating coredb" << std::endl;

    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << std::endl;
    }

    if (argc < 3)
    {
        std::cerr << "incorrect number of arguments" << std::endl;
        return 1;
    }

    fs::path output_file = fs::path(argv[3]) / "coredb.sqlite";

    if (fs::exists(output_file))
    {
        std::cout << "removing existing database... rebuilding" << std::endl;
        fs::remove(output_file);
    }

    sqlite3* db;
    sqlite3_open(output_file.string().c_str(), &db);
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    insert_migrations(argv[2], db);
    insert_translations(argv[1], db);

    sqlite3_exec(db, "END TRANSACTION", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "VACUUM", nullptr, nullptr, nullptr);
    sqlite3_close(db);

    return 0;
}
