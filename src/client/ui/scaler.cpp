#include <picotorrent/client/ui/scaler.hpp>

using picotorrent::client::ui::scaler;

scaler::scaler()
    : hdc_(GetDC(NULL))
{
    x_ = GetDeviceCaps(hdc_, LOGPIXELSX);
    y_ = GetDeviceCaps(hdc_, LOGPIXELSY);
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
    return MulDiv(x, instance().x_, 96);
}

long scaler::y(long y)
{
    return MulDiv(y, instance().y_, 96);
}
