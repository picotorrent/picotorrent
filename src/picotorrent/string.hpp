#pragma once

#include <string>
#include <vector>

namespace pt
{
    class String
    {
    public:
        static std::vector<std::string> Split(std::string const& value, std::string delim = ",");
    };
}
