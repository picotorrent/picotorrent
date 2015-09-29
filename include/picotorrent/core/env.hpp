#pragma once

#include <picotorrent/filesystem/path.hpp>

namespace picotorrent
{
namespace core
{
    class env
    {
    public:
        static filesystem::path get_user_downloads_directory();
    };
}
}
