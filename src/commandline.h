#pragma once

#include <memory>
#include <string>
#include <vector>

namespace pico
{
    class CommandLine
    {
    public:
        static std::unique_ptr<CommandLine> Parse();
        static std::unique_ptr<CommandLine> Parse(const std::wstring& cmdLine);

        std::vector<std::wstring> GetFiles();

    private:
        std::vector<std::wstring> files_;
    };
}
