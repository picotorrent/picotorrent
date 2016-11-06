#pragma once

#include <picotorrent/api.hpp>

namespace API
{
namespace IO
{
    class FileSystem : public IFileSystem
    {
    public:
        DirectoryHandle GetDirectory(DirectoryPath const& path);
        FileHandle GetFile(FilePath const& path);
    };
}
}
