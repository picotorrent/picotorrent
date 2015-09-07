#include "commandline.h"

#include <windows.h>
#include <shellapi.h>

using namespace pico;

std::unique_ptr<CommandLine> CommandLine::Parse()
{
    return Parse(GetCommandLine());
}

std::unique_ptr<CommandLine> CommandLine::Parse(const std::wstring& cmdLine)
{
    LPWSTR* argv;
    int argc;

    argv = CommandLineToArgvW(cmdLine.c_str(), &argc);

    std::unique_ptr<CommandLine> cmd = std::make_unique<CommandLine>();

    if(argc == 0 || argv == NULL)
    {
        return std::move(cmd);
    }

    for (int i = 1; i < argc; i++)
    {
        cmd->files_.push_back(argv[i]);
    }

    return std::move(cmd);
}

std::vector<std::wstring> CommandLine::GetFiles()
{
    return files_;
}
