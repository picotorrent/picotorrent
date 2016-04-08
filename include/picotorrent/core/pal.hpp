#pragma once

#include <string>
#include <vector>

namespace picotorrent
{
namespace core
{
    class pal
    {
    public:
        static std::string combine_paths(const std::string &p1, const std::string &p2);
        static void create_directories(const std::string &path);
        static bool directory_exists(const std::string &path);
        static bool file_exists(const std::string &path);
        static std::vector<std::string> get_directory_entries(const std::string &directory, const std::string &filter);
        static void remove_file(const std::string &path);
        static std::string replace_extension(const std::string &path, const std::string &new_extension);
    };
}
}
