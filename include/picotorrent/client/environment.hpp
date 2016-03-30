#pragma once

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class path;
}
}
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
        static core::filesystem::path get_data_path();
        static core::filesystem::path get_special_folder(special_folder folder);
        static core::filesystem::path get_temporary_directory();
        static bool is_installed();
    };
}
}
