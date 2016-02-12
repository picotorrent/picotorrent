#pragma once

#include <picotorrent/common.hpp>
#include <picotorrent/core/filesystem/path.hpp>
#include <vector>

namespace fs = picotorrent::core::filesystem;

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class directory
    {
    public:
        DLL_EXPORT directory(const path &p);

        DLL_EXPORT void create();
        DLL_EXPORT fs::path& path();
        DLL_EXPORT std::vector<fs::path> get_files(const fs::path &p);

    private:
        fs::path p_;
    };
}
}
}
