#pragma once

#include <functional>
#include <windows.h>

namespace picotorrent
{
namespace core
{
    class timer
    {
    public:
        timer(const std::function<void()> &callback, int interval);
        ~timer();

        void start();
        void stop();

    private:
        static void CALLBACK callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

        std::function<void()> callback_;
        int interval_;
        HANDLE hTimer_;
    };
}
}
