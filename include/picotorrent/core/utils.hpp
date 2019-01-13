#pragma once

#include <picotorrent/export.hpp>

#include <filesystem>
#include <string>

namespace pt
{
    class PICO_CORE_API Utils
    {
    public:
        static void openAndSelect(std::experimental::filesystem::path path);
        static std::wstring toHumanFileSize(int64_t bytes);
        static std::string toStdString(std::wstring const& input);
        static std::wstring toStdWString(std::string const& input);
    };
}
