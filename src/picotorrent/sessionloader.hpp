#pragma once

#include <memory>

namespace pt
{
    class Configuration;
    class Database;
    struct SessionState;

    class SessionLoader
    {
    public:
        static std::shared_ptr<SessionState> load(std::shared_ptr<Database> db, std::shared_ptr<Configuration> cfg);
    };
}
