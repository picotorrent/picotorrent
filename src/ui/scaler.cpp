#include <picotorrent/ui/scaler.hpp>

using picotorrent::ui::scaler;

scaler::scaler()
    : hdc_(GetDC(NULL)),
    dpi_x_(96),
    dpi_y_(96)
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
    return MulDiv(x, GetDeviceCaps(instance().hdc_, LOGPIXELSX), instance().dpi_x_);
}

long scaler::y(long y)
{
    return MulDiv(y, GetDeviceCaps(instance().hdc_, LOGPIXELSY), instance().dpi_y_);
}
