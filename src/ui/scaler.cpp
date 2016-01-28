#include <picotorrent/ui/scaler.hpp>

using picotorrent::ui::scaler;

scaler::scaler()
    : hdc_(GetDC(NULL))
{
    int logicalHeight = GetDeviceCaps(hdc_, VERTRES);
    int physicalHeight = GetDeviceCaps(hdc_, DESKTOPVERTRES);

    scaling_ = (float)physicalHeight / (float)logicalHeight;
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
    return (long)(x * instance().scaling_);
}

long scaler::y(long y)
{
    return (long)(y * instance().scaling_);
}
