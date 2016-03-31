#pragma once

#include <string>
#include <vector>

namespace picotorrent
{
namespace client
{
    class command_line
    {
    public:
        static command_line parse(const std::wstring &cmd);

        std::vector<std::string> files() const;
        std::vector<std::string> magnet_links() const;
        bool restart() const;
        int prev_process_id() const;

    private:
        std::vector<std::string> magnets_;
        std::vector<std::string> files_;
        bool restart_;
        int prev_process_id_;
    };
}
}
