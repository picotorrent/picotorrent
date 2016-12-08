#pragma once

#include <string>

class VersionInformation
{
public:
    static std::string GetBranch();
    static std::string GetCommitHash();
    static std::string GetCurrentVersion();
};
