#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace pt
{
namespace UI
{
namespace Dialogs
{
    class AboutDialog : public wxDialog
    {
    public:
        AboutDialog(wxWindow* parent, wxWindowID id);
        virtual ~AboutDialog();
    };
}
}
}
