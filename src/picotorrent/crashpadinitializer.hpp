#pragma once

#include <memory>

namespace pt
{
    namespace Core
    {
        class Environment;
    }

    class CrashpadInitializer
    {
    public:
        static void Initialize(std::shared_ptr<Core::Environment> env);
    };
}
