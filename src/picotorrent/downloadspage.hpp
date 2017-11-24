#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    class Translator;

    class DownloadsPage : public wxPanel
    {
    public:
        DownloadsPage(wxWindow* parent, std::shared_ptr<Translator> translator);
    };
}
