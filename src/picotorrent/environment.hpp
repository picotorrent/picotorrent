#pragma once

#include <filesystem>

namespace fs = std::experimental::filesystem::v1;

namespace pt
{
    class Environment
    {
    public:
		enum KnownFolder
		{
			LocalAppData
		};

        fs::path GetApplicationDataPath();
		fs::path Environment::GetKnownFolderPath(KnownFolder knownFolder);
		bool IsAppContainerProcess();
        bool IsInstalled();
    };
}
