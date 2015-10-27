#pragma once

namespace picotorrent
{
namespace filesystem
{
    class path;
}
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
        static filesystem::path get_data_path();

        static filesystem::path get_special_folder(special_folder folder);

        static bool is_installed();
    };
}
}
