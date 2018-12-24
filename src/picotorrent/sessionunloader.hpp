#pragma once

#include <memory>

namespace pt
{
    class Database;
    struct SessionState;

    class SessionUnloader
    {
    public:
        static void unload(std::shared_ptr<SessionState> state, std::shared_ptr<Database> db);
    };
}
