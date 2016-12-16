#pragma once

#include <string>
#include <vector>

namespace IO
{
class Directory
{
public:
    static void Create(const std::wstring& dir);
    static bool Exists(const std::wstring& path);
    static std::vector<std::wstring> GetFiles(const std::wstring& path, const std::wstring& filter);
};
}
