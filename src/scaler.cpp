#include "scaler.h"

using pico::Scaler;

Scaler::Scaler()
    : hdc_(GetDC(NULL))
{
}

Scaler::~Scaler()
{
    ReleaseDC(NULL, hdc_);
}

long Scaler::GetX(long x)
{
    return MulDiv(x, GetDeviceCaps(hdc_, LOGPIXELSX), 96);
}

long Scaler::GetY(long y)
{
    return MulDiv(y, GetDeviceCaps(hdc_, LOGPIXELSY), 96);
}
