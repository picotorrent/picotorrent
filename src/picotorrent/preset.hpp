#pragma once

#include <filesystem>
#include <string>

namespace pt
{
    struct Preset
    {
        Preset(std::string const& name)
            : name(name),
            save_path(""),
            move_completed_downloads(false),
            move_completed_path("")
        {
        }

        std::string name;
        std::experimental::filesystem::v1::path save_path;
        bool move_completed_downloads;
        std::experimental::filesystem::v1::path move_completed_path;
    };
}
