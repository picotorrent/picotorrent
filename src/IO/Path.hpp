#pragma once

#include <string>

namespace IO
{
class Path
{
public:
    static std::wstring Combine(const std::wstring& path1, const std::wstring& path2);
    static std::wstring ReplaceExtension(const std::wstring& path, const std::wstring& ext);
};
}
