#pragma once

#include "windows.h"

#include <string>
#include <vector>

struct CommandLine
{
    std::vector<std::wstring> files;
    std::vector<std::wstring> magnet_links;

    static CommandLine Parse(const std::wstring& cmd);
};
