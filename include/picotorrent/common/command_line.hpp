#pragma once

#include <string>
#include <vector>

#include <picotorrent/export.hpp>

namespace picotorrent
{
namespace common
{
    class command_line
    {
    public:
        DLL_EXPORT static command_line parse(const std::wstring &cmd, bool skip_first = false);

        DLL_EXPORT bool alloc_console() const;
        DLL_EXPORT bool daemon() const;
        DLL_EXPORT std::vector<std::string> files() const;
        DLL_EXPORT std::vector<std::string> magnet_links() const;
        DLL_EXPORT bool restart() const;
        DLL_EXPORT int prev_process_id() const;

    private:
        std::vector<std::string> magnets_;
        std::vector<std::string> files_;
        bool alloc_console_;
        bool daemon_;
        bool restart_;
        int prev_process_id_;
    };
}
}
