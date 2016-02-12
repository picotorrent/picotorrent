#pragma once

#include <functional>
#include <picotorrent/common.hpp>
#include <windows.h>

namespace picotorrent
{
namespace core
{
    class timer
    {
    public:
        DLL_EXPORT timer(const std::function<void()> &callback, int interval);
        DLL_EXPORT ~timer();

        DLL_EXPORT void start();
        DLL_EXPORT void stop();

    private:
        static void CALLBACK callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

        std::function<void()> callback_;
        int interval_;
        HANDLE hTimer_;
    };
}
}
