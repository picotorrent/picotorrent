#pragma once

#include <filesystem>

#include <picotorrent/export.hpp>

namespace pt
{
    class Environment
    {
    public:
        enum KnownFolder
        {
            LocalAppData,
            UserDownloads
        };

        std::experimental::filesystem::path PICO_API getApplicationDataPath();
        std::experimental::filesystem::path PICO_API getApplicationPath();
        std::experimental::filesystem::path PICO_API getDatabaseFilePath();
        std::experimental::filesystem::path PICO_API getKnownFolderPath(KnownFolder knownFolder);
        bool PICO_API isAppContainerProcess();
        bool PICO_API isInstalled();
    };
}
