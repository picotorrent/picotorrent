#pragma once

#include <picotorrent/filesystem/path.hpp>
#include <string>
#include <vector>

namespace picotorrent
{
namespace app
{
    class command_line
    {
    public:
        static command_line parse(const std::wstring &cmd);

        std::vector<filesystem::path> files() const;
        std::vector<std::wstring> magnet_links() const;
        bool restart() const;
        int prev_process_id() const;

    private:
        std::vector<std::wstring> magnets_;
        std::vector<filesystem::path> files_;
        bool restart_;
        int prev_process_id_;
    };
}
}
