#pragma once

#include <string>

namespace boost
{
    namespace filesystem
    {
        class path;
    }
}

namespace pico
{
    class Path
    {
    public:
        static boost::filesystem::path GetDefaultDownloadsPath();
        static boost::filesystem::path GetLogPath();
        static boost::filesystem::path GetStatePath();
        static boost::filesystem::path GetTorrentsPath();
    };
}
