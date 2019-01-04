#pragma once

#include <filesystem>

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

        std::experimental::filesystem::path getApplicationDataPath();
        std::experimental::filesystem::path getDatabaseFilePath();
        std::experimental::filesystem::path getKnownFolderPath(KnownFolder knownFolder);
        bool isAppContainerProcess();
        bool isInstalled();
    };
}
