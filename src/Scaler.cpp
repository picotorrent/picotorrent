#include "Scaler.hpp"

Scaler::Scaler()
    : hdc_(GetDC(NULL))
{
    x_ = GetDeviceCaps(hdc_, LOGPIXELSX);
    y_ = GetDeviceCaps(hdc_, LOGPIXELSY);
}

Scaler::~Scaler()
{
    ReleaseDC(NULL, hdc_);
}

Scaler& Scaler::GetInstance()
{
    static Scaler inst;
    return inst;
}

long Scaler::X(long x)
{
    return MulDiv(x, GetInstance().x_, 96);
}

long Scaler::Y(long y)
{
    return MulDiv(y, GetInstance().y_, 96);
}
