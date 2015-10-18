#include <picotorrent/ui/scaler.hpp>

using picotorrent::ui::scaler;

scaler::scaler()
    : hdc_(GetDC(NULL))
{
}

scaler::~scaler()
{
    ReleaseDC(NULL, hdc_);
}

scaler& scaler::instance()
{
    static scaler inst;
    return inst;
}

long scaler::x(long x)
{
    return MulDiv(x, GetDeviceCaps(instance().hdc_, LOGPIXELSX), 96);
}

long scaler::y(long y)
{
    return MulDiv(y, GetDeviceCaps(instance().hdc_, LOGPIXELSY), 96);
}
