#pragma once

#include <filesystem>

namespace pt
{
    struct Preset
    {
        std::string name;
        std::experimental::filesystem::v1::path save_path;
        bool move_completed_downloads;
        std::experimental::filesystem::v1::path move_completed_path;
    };
}
