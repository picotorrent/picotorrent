#pragma once

#include <picotorrent/filesystem/path.hpp>
#include <vector>

namespace fs = picotorrent::filesystem;

namespace picotorrent
{
namespace filesystem
{
    class directory
    {
    public:
        directory(const path &p);

        void create();
        fs::path& path();
        std::vector<fs::path> get_files(const fs::path &p);

    private:
        fs::path p_;
    };
}
}
