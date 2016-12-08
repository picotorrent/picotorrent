#pragma once

#include <vector>

namespace Diagnostics
{
    struct StackFrame;

    class StackTrace
    {
    public:
        std::vector<StackFrame> frames;

        static StackTrace Capture(int depth = 0, void* context = nullptr);
    };
}
