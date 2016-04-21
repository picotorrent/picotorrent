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
        static std::string extract_public_key(const std::string &certificate_file);
        static std::vector<char> generate();
    };
}
}
}
