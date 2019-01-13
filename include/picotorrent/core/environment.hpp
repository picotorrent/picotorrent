#pragma once

#include <filesystem>

#include <picotorrent/export.hpp>

namespace pt
{
    class PICO_CORE_API Environment
    {
    public:
        enum KnownFolder
        {
            LocalAppData,
            UserDownloads
        };

        std::experimental::filesystem::path getApplicationDataPath();
        std::experimental::filesystem::path getApplicationPath();
        std::experimental::filesystem::path getDatabaseFilePath();
        std::experimental::filesystem::path getKnownFolderPath(KnownFolder knownFolder);
        bool isAppContainerProcess();
        bool isInstalled();
    };
}
