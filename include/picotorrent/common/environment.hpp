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
        user_downloads
    };

    class environment
    {
    public:
        static filesystem::path get_special_folder(special_folder folder);
    };
}
}
