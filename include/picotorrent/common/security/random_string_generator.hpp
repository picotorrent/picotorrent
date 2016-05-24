#pragma once

#include <string>

namespace picotorrent
{
namespace common
{
namespace security
{
    class random_string_generator
    {
    public:
        std::string generate(int len);
    };
}
}
}
