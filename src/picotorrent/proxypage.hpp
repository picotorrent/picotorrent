#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <memory>

namespace pt
{
    class Translator;

    class ProxyPage : public wxPanel
    {
    public:
        ProxyPage(wxWindow* parent, std::shared_ptr<Translator> translator);

    private:
    };
}
