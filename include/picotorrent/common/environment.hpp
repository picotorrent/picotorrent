#pragma once

#include <string>

#include <picotorrent/export.hpp>

namespace picotorrent
{
namespace common
{
    enum special_folder
    {
        user_downloads,
        local_app_data
    };

    class environment
    {
    public:
        DLL_EXPORT static std::string get_data_path();
        DLL_EXPORT static std::string get_special_folder(special_folder folder);
        DLL_EXPORT static std::string get_temporary_directory();
        DLL_EXPORT static bool is_installed();
    };
}
}
