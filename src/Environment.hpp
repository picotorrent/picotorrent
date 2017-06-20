#pragma once

#include "stdafx.h"

#include <filesystem>
#include <string>

namespace fs = std::experimental::filesystem::v1;

class Environment
{
public:
    static fs::path GetApplicationPath();
    static fs::path GetDataPath();
    static fs::path GetKnownFolderPath(const KNOWNFOLDERID& rfid);
    static bool IsAppContainerProcess();
    static bool IsInstalled();
};
