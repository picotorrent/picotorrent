#include "SleepManager.hpp"

#include <windows.h>

#include "Log.hpp"

SleepManager::SleepManager()
    : m_isPrevented(false)
{
}

void SleepManager::AllowSleep()
{
    if (m_isPrevented)
    {
        // Allow sleeping
        LOG(Info) << "Allowing computer to sleep";
        EXECUTION_STATE state = SetThreadExecutionState(ES_CONTINUOUS);
        if (state == NULL) 
        {
            LOG(Error) << "Could not allow computer to sleep";
            return;
        }
        m_isPrevented = false;
    }
}

void SleepManager::PreventSleep()
{
    if (!m_isPrevented)
    {
        // Prevent sleeping
        LOG(Info) << "Preventing computer from sleeping";
        EXECUTION_STATE state = SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED);
        if (state == NULL) 
        {
            LOG(Error) << "Could not prevent computer from sleeping.";
            return;
        }
        m_isPrevented = true;
    }
}
