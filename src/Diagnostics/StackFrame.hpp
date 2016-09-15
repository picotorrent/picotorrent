#pragma once

#include <string>

namespace Diagnostics
{
    struct StackFrame
    {
        std::string address;
        std::string name;
        int64_t displacement;
        std::string fileName;
        int lineNumber;
    };
}
