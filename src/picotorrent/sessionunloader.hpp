#pragma once

#include <memory>

namespace pt
{
    class Environment;
    struct SessionState;

    class SessionUnloader
    {
    public:
        static void Unload(std::shared_ptr<SessionState> state, std::shared_ptr<Environment> env);
    };
}
