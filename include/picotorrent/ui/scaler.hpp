#pragma once

#include <windows.h>

namespace picotorrent
{
namespace ui
{
    class scaler
    {
    public:
        explicit scaler();
        ~scaler();

        static long x(long x);
        static long y(long y);

    private:
        static scaler& instance();
        const HDC hdc_;
        UINT dpi_x_;
        UINT dpi_y_;
    };
}
}
