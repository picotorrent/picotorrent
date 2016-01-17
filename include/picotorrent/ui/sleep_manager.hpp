#pragma once

#include <stdint.h>
#include <windows.h>

namespace picotorrent
{
namespace ui
{
	class sleep_manager
	{
    public:
        sleep_manager();
        ~sleep_manager();

        void refresh(bool hasActiveDownloads);

    private:
        bool isSleepPrevented_;
    };
}
}