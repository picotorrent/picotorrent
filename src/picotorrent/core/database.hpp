#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_context sqlite3_context;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3_value sqlite3_value;


namespace pt::Core
{
    class Environment;

    class Database
    {
    public:
        struct Statement
        {
            friend class Database;

            ~Statement();
            void Bind(int idx, int value);
            void Bind(int idx, std::optional<int> value);
            void Bind(int idx, std::string const& value);
            void Bind(int idx, std::vector<char> const& value);
            bool Execute();
            void GetBlob(int idx, std::vector<char>& res);
            bool GetBool(int idx);
            int GetInt(int idx);
            std::string GetString(int idx);
            bool Read();

        private:
            Statement(sqlite3_stmt* stmt);
            sqlite3_stmt* m_stmt;
        };

        Database(std::shared_ptr<Environment> env);
        ~Database();

        void Execute(std::string const& sql);
        bool Migrate();
        std::shared_ptr<Statement> CreateStatement(std::string const& sql);

    private:
        static void GetKnownFolderPath(sqlite3_context* ctx, int argc, sqlite3_value** argv);
        static void GetUserDefaultUILanguage(sqlite3_context* ctx, int argc, sqlite3_value** argv);

        bool MigrationExists(std::string const& name);

        sqlite3* m_db;
        std::shared_ptr<Environment> m_env;
    };
}

