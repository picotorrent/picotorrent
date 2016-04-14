#pragma once

#include <string>

namespace picotorrent
{
namespace client
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
