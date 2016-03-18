#pragma once

#include <picotorrent/common.hpp>

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class path;
}
    enum special_folder
    {
        user_downloads,
        local_app_data
    };

    class environment
    {
    public:
        DLL_EXPORT static core::filesystem::path get_data_path();

        DLL_EXPORT static core::filesystem::path get_special_folder(special_folder folder);

        DLL_EXPORT static core::filesystem::path get_temporary_directory();

        DLL_EXPORT static bool is_installed();
    };
}
}
