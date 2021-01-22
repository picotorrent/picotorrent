#pragma once

#include <nlohmann/json.hpp>

namespace pt::commands
{
    class command
    {
    public:
        virtual nlohmann::json execute(nlohmann::json const& params) = 0;
    };
}
