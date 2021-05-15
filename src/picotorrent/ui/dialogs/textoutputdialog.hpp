#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>

namespace pt
{
namespace UI
{
namespace Dialogs
{
    class TextOutputDialog : public wxDialog
    {
    public:
        TextOutputDialog(wxWindow* parent, wxWindowID id, std::wstring const& title, std::wstring const& desc);
        virtual ~TextOutputDialog();

        void SetOutputText(std::string const& text);

    private:
        wxTextCtrl* m_outputText;
    };
}
}
}
