#include <picotorrent/ui/sleep_manager.hpp>
#include <picotorrent/logging/log.hpp>

using picotorrent::ui::sleep_manager;

sleep_manager::sleep_manager()
    : isSleepPrevented_(false)
{
}

sleep_manager::~sleep_manager()
{
    if (isSleepPrevented_)
    {
        SetThreadExecutionState(ES_CONTINUOUS);
    }
}

void sleep_manager::refresh(uint32_t active)
{
    if (active == 0)
    {
        if (isSleepPrevented_)
        {
            // Allow sleeping
            LOG(info) << "Allowing computer to sleep";
            EXECUTION_STATE state = SetThreadExecutionState(ES_CONTINUOUS);
            if (state == NULL) {
                LOG(error) << "Could not allow computer to sleep.";
                return;
            }
            isSleepPrevented_ = false;
        }
    }
    else
    {
        if (!isSleepPrevented_)
        {
            // Prevent sleeping
            LOG(info) << "Preventing computer from sleeping";
            EXECUTION_STATE state = SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
            if (state == NULL) {
                LOG(error) << "Could not prevent computer from sleeping.";
                return;
            }
            isSleepPrevented_ = true;
        }
    }
}