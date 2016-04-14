#pragma once

#include <vector>

namespace picotorrent
{
namespace client
{
namespace security
{
    class certificate_manager
    {
    public:
        static std::vector<char> generate();
    };
}
}
}
