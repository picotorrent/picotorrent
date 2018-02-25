#pragma once

#include <filesystem>

namespace pt
{
    class Utils_Win32
    {
    public:
        static void OpenAndSelect(std::experimental::filesystem::v1::path path);
    };
}
