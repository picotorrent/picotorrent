#pragma once

#include <vector>

#include <picotorrent/export.hpp>

namespace picotorrent
{
namespace common
{
namespace security
{
    class certificate_manager
    {
    public:
        DLL_EXPORT static std::string extract_public_key(const std::string &certificate_file);
        DLL_EXPORT static std::vector<char> generate();
    };
}
}
}
