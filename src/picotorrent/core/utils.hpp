#pragma once

#include <filesystem>
#include <string>

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif

namespace pt
{
    class Utils
    {
    public:
        static void openAndSelect(std::filesystem::path path);
        static std::wstring toHumanFileSize(int64_t bytes);
        static std::string toStdString(std::wstring const& input);
        static std::wstring toStdWString(std::string const& input);
    };
}
