#pragma once

#include <string>
#include <vector>

namespace pico
{
    namespace io
    {
        class File
        {
        public:
            static void Delete(const std::wstring &path);
            static bool Exists(const std::wstring& path);
            static bool ReadBuffer(const std::string& path, std::vector<char>& buffer);
            static bool ReadBuffer(const std::wstring& path, std::vector<char>& buffer);
            static bool WriteBuffer(const std::string& path, const std::vector<char>& buffer);
            static bool WriteBuffer(const std::wstring& path, const std::vector<char>& buffer);
        };
    }
}
