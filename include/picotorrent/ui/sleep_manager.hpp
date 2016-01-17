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
        explicit sleep_manager();
        ~sleep_manager();

        void refresh(uint32_t active);

    private:
        bool isSleepPrevented_;
    };
}
}