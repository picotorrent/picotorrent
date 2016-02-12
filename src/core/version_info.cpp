#include <picotorrent/core/version_info.hpp>

using picotorrent::core::version_info;

std::string version_info::git_branch()
{
    return "f/plugins";
}

std::string version_info::git_commit_hash()
{
    return "1c283a0";
}

std::string version_info::current_version()
{
    return "0.6.0";
}
