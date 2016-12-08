#pragma once

#include <picotorrent/api.hpp>

namespace API
{
namespace IO
{
    class File : public IFile
    {
    public:
        File(FilePath const& path);

        FileHandle ChangeExtension(std::wstring const& extension);
        void Delete();
        bool Exists();
        size_t Length();
        std::vector<char> ReadAllBytes(std::error_code& ec);

    private:
        FilePath m_path;
    };
}
}
