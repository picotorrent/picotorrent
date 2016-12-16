#pragma once

#include <picotorrent/api.hpp>

namespace API
{
namespace IO
{
    class Directory : public IDirectory
    {
    public:
        Directory(DirectoryPath const& path);

        bool Exists();
        FileHandleCollection GetFiles(std::wstring const& filter);

    private:
        DirectoryPath m_path;
    };
}
}
