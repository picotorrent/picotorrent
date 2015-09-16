#pragma once

#include <string>
#include <vector>

namespace pico
{
    namespace io
    {
        class Directory
        {
        public:
            static void CreateDirectories(const std::wstring &path);
            static bool Exists(const std::wstring& path);
            static std::vector<std::wstring> GetFiles(const std::wstring& path, const std::wstring &filter);
        };
    }
}
