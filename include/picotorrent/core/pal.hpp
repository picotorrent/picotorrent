#pragma once

#include <string>
#include <vector>

#include <picotorrent/common.hpp>

namespace picotorrent
{
namespace core
{
    class pal
    {
    public:
        DLL_EXPORT static std::string combine_paths(const std::string &p1, const std::string &p2);
        DLL_EXPORT static void create_directories(const std::string &path);
        DLL_EXPORT static bool directory_exists(const std::string &path);
        DLL_EXPORT static bool file_exists(const std::string &path);
        DLL_EXPORT static std::vector<std::string> get_directory_entries(const std::string &directory, const std::string &filter);
        DLL_EXPORT static void remove_file(const std::string &path);
        DLL_EXPORT static std::string replace_extension(const std::string &path, const std::string &new_extension);
    };
}
}
