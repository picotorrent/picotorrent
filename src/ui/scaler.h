#pragma once

#include <windows.h>

namespace pico
{
    namespace ui
    {
        class Scaler
        {
        public:
            explicit Scaler();
            ~Scaler();

            long GetX(long x);
            long GetY(long y);

        private:
            const HDC hdc_;
        };
    }
}
