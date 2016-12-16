#pragma once

class SleepManager
{
public:
    SleepManager();

    void AllowSleep();
    void PreventSleep();

private:
    bool m_isPrevented;
};
