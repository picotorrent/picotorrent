#include <picotorrent/app/command_line.hpp>

#include <windows.h>
#include <shellapi.h>

using picotorrent::app::command_line;

command_line command_line::parse(const std::wstring &cmd)
{
    int argv;
    LPWSTR *argc = CommandLineToArgvW(cmd.c_str(), &argv);

    command_line cl;

    for (int i = 0; i < argv; i++)
    {
        std::wstring arg(argc[i]);

        if (arg.empty())
        {
            continue;
        }

        if (arg.substr(0, 10) == L"magnet:?xt")
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

std::vector<picotorrent::filesystem::path> command_line::files() const
{
    return files_;
}

std::vector<std::wstring> command_line::magnet_links() const
{
    return magnets_;
}
