#pragma once

#include <filesystem>

#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>

namespace fs = std::experimental::filesystem::v1;

namespace pt
{
    class Environment
    {
    public:
        fs::path GetApplicationDataPath();
		fs::path Environment::GetKnownFolderPath(KNOWNFOLDERID const& rfid);
		bool IsAppContainerProcess();
        bool IsInstalled();
    };
}
