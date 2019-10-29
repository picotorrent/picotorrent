#pragma once

#include <memory>

namespace pt
{
    class Environment;

    class CrashpadInitializer
    {
    public:
        static void init(std::shared_ptr<Environment> env);
    };
}
