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
            static void ReadBuffer(const std::string& path, std::vector<char>& buffer);
            static void ReadBuffer(const std::wstring& path, std::vector<char>& buffer);
            static void WriteBuffer(const std::string& path, const std::vector<char>& buffer);
            static void WriteBuffer(const std::wstring& path, const std::vector<char>& buffer);
        };
    }
}
