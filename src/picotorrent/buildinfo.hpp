#pragma once

#include <string>

namespace pt
{
    class BuildInfo
    {
    public:
        static std::string branch();
        static std::string commitish();
        static std::string version();
    };
}
