#pragma once

#include <chrono>
#include <filesystem>

namespace pt
{
namespace Core
{
    class Environment
    {
    public:
        enum KnownFolder
        {
            LocalAppData,
            UserDownloads
        };

        static std::shared_ptr<Environment> Create();

        std::filesystem::path GetApplicationDataPath();
        std::filesystem::path GetApplicationPath();
        std::filesystem::path GetDatabaseFilePath();
        std::filesystem::path GetKnownFolderPath(KnownFolder knownFolder);
        std::filesystem::path GetLogFilePath();
        bool IsAppContainerProcess();
        bool IsInstalled();

    private:
        Environment();
        std::chrono::system_clock::time_point m_startupTime;
    };
}
}
