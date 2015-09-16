#pragma once

#include <string>

namespace pico
{
    namespace io
    {
        class Path
        {
        public:
            static std::wstring ChangeExtension(const std::wstring &file, const std::wstring &extension);
            static std::wstring Combine(const std::wstring& path1, const std::wstring& path2);
            static std::wstring GetDefaultDownloadsPath();
            static std::wstring GetExtension(const std::wstring &file);
            static std::wstring GetLogPath();
            static std::wstring GetStatePath();
            static std::wstring GetTorrentsPath();
        };
    }
}
