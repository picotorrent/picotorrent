#pragma once

#include <filesystem>

namespace fs = std::experimental::filesystem;

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

        fs::path getApplicationDataPath();
        fs::path getKnownFolderPath(KnownFolder knownFolder);
        bool isAppContainerProcess();
        bool isInstalled();
    };
}
