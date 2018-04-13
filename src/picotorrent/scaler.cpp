
#include "scaler.hpp"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

using pt::Scaler;

Scaler::Scaler()
{
    x_ = wxGetDisplayPPI().GetWidth();
    y_ = wxGetDisplayPPI().GetHeight();
}

Scaler& Scaler::GetInstance()
{
    static Scaler inst;
    return inst;
}

long Scaler::X(long x)
{
    return wxMulDivInt32(x, GetInstance().x_, 96);
}

long Scaler::Y(long y)
{
    return wxMulDivInt32(y, GetInstance().y_, 96);
}
