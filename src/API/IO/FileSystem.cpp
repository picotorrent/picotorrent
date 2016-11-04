#include "FileSystem.hpp"

#include "Directory.hpp"
#include "File.hpp"

using API::IO::FileSystem;

DirectoryHandle FileSystem::GetDirectory(DirectoryPath const& path)
{
    return std::make_shared<Directory>(path);
}

FileHandle FileSystem::GetFile(FilePath const& path)
{
    return std::make_shared<File>(path);
}
