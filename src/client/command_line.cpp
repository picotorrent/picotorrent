#include <picotorrent/client/command_line.hpp>

#include <picotorrent/client/string_operations.hpp>

#include <windows.h>
#include <shellapi.h>

using picotorrent::client::to_string;
using picotorrent::client::command_line;

command_line command_line::parse(const std::wstring &cmd)
{
    int argv;
    LPWSTR *argc = CommandLineToArgvW(cmd.c_str(), &argv);

    command_line cl;
    cl.restart_ = false;

    for (int i = 0; i < argv; i++)
    {
        std::wstring arg(argc[i]);

        if (arg.empty())
        {
            continue;
        }

        if (arg.substr(0, 10) == L"magnet:?xt")
        {
            cl.magnets_.push_back(to_string(arg));
        }
        else
        {
            cl.files_.push_back(to_string(arg));
        }

        if (arg == L"--restart")
        {
            cl.restart_ = true;
            cl.prev_process_id_ = std::stoi(argc[i + 1]);
        }
    }

    return cl;
}

std::vector<std::string> command_line::files() const
{
    return files_;
}

std::vector<std::string> command_line::magnet_links() const
{
    return magnets_;
}

bool command_line::restart() const
{
    return restart_;
}

int command_line::prev_process_id() const
{
    return prev_process_id_;
}
