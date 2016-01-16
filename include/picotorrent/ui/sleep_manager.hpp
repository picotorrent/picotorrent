#pragma once

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

        void refresh() const;

    private:
        bool isSleeping;
    };
}
}