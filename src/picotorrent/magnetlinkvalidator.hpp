#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
    class MagnetLinkValidator : public wxTextValidator
    {
    public:
        wxObject* Clone() const wxOVERRIDE;
        bool Validate(wxWindow* WXUNUSED(parent)) wxOVERRIDE;
    };
}
