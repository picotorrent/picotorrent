#pragma once

#include "stdafx.h"

#include <string>

class Environment
{
public:
    static std::wstring GetApplicationPath();
    static std::wstring GetDataPath();
    static bool IsInstalled();
};
