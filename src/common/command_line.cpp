#include <picotorrent/common/command_line.hpp>

#include <picotorrent/common/string_operations.hpp>

#include <windows.h>
#include <shellapi.h>

using picotorrent::common::command_line;
using picotorrent::common::to_string;

command_line command_line::parse(const std::wstring &cmd, bool skip_first)
{
    if (cmd.empty())
    {
        return command_line();
    }

    int argv;
    LPWSTR *argc = CommandLineToArgvW(cmd.c_str(), &argv);

    command_line cl;
    cl.alloc_console_ = false;
    cl.daemon_ = false;
    cl.raw_ = cmd;
    cl.restart_ = false;

    for (int i = 0; i < argv; i++)
    {
        if (skip_first && i == 0)
        {
            continue;
        }

        std::string arg = to_string(argc[i]);

        if (arg.empty())
        {
            continue;
        }

        if (arg == "--alloc-console")
        {
            cl.alloc_console_ = true;
            continue;
        }

        if (arg == "--daemon")
        {
            cl.daemon_ = true;
            continue;
        }

        if (arg == "--restart")
        {
            cl.restart_ = true;
            cl.prev_process_id_ = std::stoi(argc[i + 1]);
            i += 1;
            continue;
        }

        if (arg.substr(0, 10) == "magnet:?xt")
        {
            cl.magnets_.push_back(arg);
        }
        else
        {
            cl.files_.push_back(arg);
        }
    }

    return cl;
}

bool command_line::alloc_console() const
{
    return alloc_console_;
}

bool command_line::daemon() const
{
    return daemon_;
}

std::vector<std::string> command_line::files() const
{
    return files_;
}

std::vector<std::string> command_line::magnet_links() const
{
    return magnets_;
}

std::wstring command_line::raw() const
{
    return raw_;
}

bool command_line::restart() const
{
    return restart_;
}

int command_line::prev_process_id() const
{
    return prev_process_id_;
}
