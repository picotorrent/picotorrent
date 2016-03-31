#pragma once

#include <string>

namespace picotorrent
{
namespace client
{
    enum special_folder
    {
        user_downloads,
        local_app_data
    };

    class environment
    {
    public:
        static std::string get_data_path();
        static std::string get_special_folder(special_folder folder);
        static std::string get_temporary_directory();
        static bool is_installed();
    };
}
}
