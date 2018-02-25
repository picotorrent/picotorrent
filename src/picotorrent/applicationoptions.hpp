#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>
#include <string>

namespace pt
{
    struct ApplicationOptions
    {
        wxArrayString files;
        wxArrayString magnet_links;

        static std::shared_ptr<ApplicationOptions> JsonDecode(wxString json);
        static wxString JsonEncode(std::shared_ptr<ApplicationOptions> opts);
    };
}
