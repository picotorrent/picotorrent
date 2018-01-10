#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
    struct ApplicationOptions
    {
        wxArrayString files;
        wxArrayString magnet_links;
    };
}
