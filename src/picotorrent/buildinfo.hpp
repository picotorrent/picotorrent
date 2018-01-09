#pragma once

#include <string>

namespace pt
{
    class BuildInfo
    {
    public:
        static std::string Branch();
        static std::string Commitish();
        static std::string Version();
    };
}
