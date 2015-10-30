#pragma once

#include <string>

namespace picotorrent
{
namespace filesystem
{
    class path
    {
    public:
        path(const std::wstring &p);
        path(const wchar_t *p);

        path combine(const path &p) const;
        bool exists() const;
        path filename() const;
        bool is_directory();
        bool is_file();
        path replace_extension(const std::wstring &ext);
        std::wstring to_string() const;

    private:
        std::wstring p_;
    };
}
}
