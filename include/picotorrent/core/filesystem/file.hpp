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
    class path;

    class file
    {
    public:
        DLL_EXPORT file(const fs::path &p);

        DLL_EXPORT fs::path& path();
        DLL_EXPORT void read_all(std::vector<char> &buf);
        DLL_EXPORT void remove();
        DLL_EXPORT void write_all(const std::vector<char> &buf);

    private:
        fs::path p_;
    };
}
}
}
