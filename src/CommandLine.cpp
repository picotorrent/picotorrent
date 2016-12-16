#include "CommandLine.hpp"

#include <shellapi.h>

CommandLine CommandLine::Parse(const std::wstring& cmd)
{
    if (cmd.empty())
    {
        return CommandLine();
    }

    int argc;
    LPWSTR* argv = CommandLineToArgvW(cmd.c_str(), &argc);

    CommandLine cl;

    for (int i = 0; i < argc; i++)
    {
        std::wstring arg = argv[i];

        if (arg.substr(0, 10) == TEXT("magnet:?xt"))
        {
            cl.magnet_links.push_back(arg);
        }
        else
        {
            cl.files.push_back(arg);
        }
    }

    return cl;
}
