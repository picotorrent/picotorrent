#pragma once

#include <functional>

namespace Commands
{
    struct InvokeCommand
    {
        std::function<void()> callback;
    };
}
