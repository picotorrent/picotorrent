#pragma once

#include <memory>
#include <string>

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_context sqlite3_context;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef struct sqlite3_value sqlite3_value;

namespace pt
{
    class Database
    {
    public:
        struct Statement
        {
            friend class Database;

            ~Statement();
            void bind(int idx, std::string const& value);
            void execute();
            int getInt(int idx);
            std::string getString(int idx);

        private:
            Statement(sqlite3_stmt* stmt);
            sqlite3_stmt* m_stmt;
        };

        Database(std::string const& fileName);
        ~Database();

        void execute(std::string const& sql);
        bool migrate();
        std::shared_ptr<Statement> statement(std::string const& sql);

    private:
        static void getKnownFolderPath(sqlite3_context* ctx, int argc, sqlite3_value** argv);
        static void getUserDefaultUILanguage(sqlite3_context* ctx, int argc, sqlite3_value** argv);

        bool migrationExists(std::string const& name);

        sqlite3* m_db;
    };
}
