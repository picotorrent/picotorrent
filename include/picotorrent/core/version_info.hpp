#pragma once

#include <picotorrent/common.hpp>
#include <string>

namespace picotorrent
{
namespace core
{
    class version_info
    {
    public:
        DLL_EXPORT static std::string git_branch();
        DLL_EXPORT static std::string git_commit_hash();
        DLL_EXPORT static std::string current_version();
    };
}
}
