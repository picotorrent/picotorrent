#pragma once

#include <memory>

namespace pt
{
    class Environment;
    struct SessionState;

    class SessionLoader
    {
    public:
        static std::shared_ptr<SessionState> Load(std::shared_ptr<Environment> env);
    };
}
