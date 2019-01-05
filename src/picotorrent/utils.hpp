#pragma once

#include <QString>

#include <filesystem>

namespace pt
{
    class Utils
    {
    public:
        static void openAndSelect(std::experimental::filesystem::path path);
        static QString ToHumanFileSize(int64_t bytes);
    };
}
