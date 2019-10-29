#pragma once

#include <chrono>
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

        static std::shared_ptr<Environment> create();

        std::filesystem::path getApplicationDataPath();
        std::filesystem::path getApplicationPath();
        std::filesystem::path getDatabaseFilePath();
        std::filesystem::path getKnownFolderPath(KnownFolder knownFolder);
        std::filesystem::path getLogFilePath();
        bool isAppContainerProcess();
        bool isInstalled();

    private:
        Environment();
        std::chrono::system_clock::time_point m_startupTime;
    };
}
