#pragma once

#include <picotorrent/common.hpp>
#include <string>

namespace picotorrent
{
namespace core
{
namespace filesystem
{
    class path
    {
    public:
        DLL_EXPORT path(const std::wstring &p);
        DLL_EXPORT path(const wchar_t *p);

        DLL_EXPORT path combine(const path &p) const;
        DLL_EXPORT bool exists() const;
        DLL_EXPORT path filename() const;
        DLL_EXPORT bool is_directory();
        DLL_EXPORT bool is_file();
        DLL_EXPORT path replace_extension(const std::wstring &ext);
        DLL_EXPORT std::wstring to_string() const;

    private:
        std::wstring p_;
    };
}
}
}
