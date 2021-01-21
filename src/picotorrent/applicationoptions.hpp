#pragma once

#include <string>
#include <vector>

namespace pt
{
    struct CommandLineOptions
    {
        CommandLineOptions() : pid(-1), silent(false) {}
        long pid;
        bool silent;
        std::vector<std::string> files;
        std::vector<std::string> magnets;
        std::string save_path;
    };
}
