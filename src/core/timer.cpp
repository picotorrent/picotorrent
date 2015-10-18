#include <picotorrent/core/timer.hpp>

using picotorrent::core::timer;

timer::timer(const std::function<void()> &callback, int interval)
    : callback_(callback)
    , interval_(interval)
    , hTimer_(NULL)
{
}

timer::~timer()
{
    if(hTimer_ != NULL)
    {
        stop();
    }
}

void timer::start()
{
    CreateTimerQueueTimer(
        &hTimer_,
        NULL,
        &timer::callback,
        this,
        0,
        interval_,
        WT_EXECUTEDEFAULT);
}

void timer::stop()
{
    DeleteTimerQueueTimer(
        NULL,
        hTimer_,
        INVALID_HANDLE_VALUE);

    hTimer_ = NULL;
}

void timer::callback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
    timer* tmr = reinterpret_cast<timer*>(lpParameter);
    tmr->callback_();
}
