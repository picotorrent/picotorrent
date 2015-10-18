#pragma once

#include <picotorrent/filesystem/path.hpp>
#include <vector>

namespace fs = picotorrent::filesystem;

namespace picotorrent
{
namespace filesystem
{
    class path;

    class file
    {
    public:
        file(const fs::path &p);

        fs::path& path();
        void read_all(std::vector<char> &buf);
        void write_all(const std::vector<char> &buf);

    private:
        fs::path p_;
    };
}
}
