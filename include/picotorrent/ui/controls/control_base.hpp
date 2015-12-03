#pragma once

#include <windows.h>

namespace picotorrent
{
namespace ui
{
namespace controls
{
    class control_base
    {
    public:
        control_base(HWND handle);

        HWND handle() const;

    private:
        HWND handle_;
    };
}
}
}
