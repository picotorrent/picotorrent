#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/ipc.h>

namespace pt
{
namespace UI
{
    class MainFrame;
}
namespace IPC
{
    class Server : public wxServer
    {
    public:
        Server(UI::MainFrame* mainFrame);
        virtual ~Server();

        virtual wxConnectionBase* OnAcceptConnection(const wxString& topic) wxOVERRIDE;

    private:
        UI::MainFrame* m_frame;
    };
}
}
