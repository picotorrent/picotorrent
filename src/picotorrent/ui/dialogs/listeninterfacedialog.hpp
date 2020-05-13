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
    class ListenInterfaceDialog : public wxDialog
    {
    public:
        ListenInterfaceDialog(wxWindow* parent, wxWindowID id, std::string address = std::string(), int port = -1);
        virtual ~ListenInterfaceDialog();

        std::string GetAddress();
        int GetPort();

    private:
        void LoadAdapters();

        wxChoice* m_adapters;
        wxTextCtrl* m_port;
    };
}
}
}
