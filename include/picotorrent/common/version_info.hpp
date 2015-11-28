#pragma once

#include <string>

namespace picotorrent
{
namespace common
{
    class version_info
    {
    public:
        static std::string git_branch();
        static std::string git_commit_hash();
        static std::string current_version();
    };
}
}
