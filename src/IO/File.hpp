#pragma once

#include <string>
#include <vector>

namespace IO
{
class File
{
public:
    static void Delete(const std::wstring& path);
    static bool Exists(const std::wstring &path);
    static std::vector<char> ReadAllBytes(const std::wstring &path, std::error_code& ec);
    static void WriteAllBytes(const std::wstring& path, const std::vector<char>& buf, std::error_code& ec);
};
}
