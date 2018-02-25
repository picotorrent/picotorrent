#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/ipc.h>

namespace pt
{
    class MainFrame;

    namespace ipc
    {
        class Server : public wxServer
        {
        public:
            Server(MainFrame* mainFrame);

            virtual wxConnectionBase* OnAcceptConnection(const wxString& topic) wxOVERRIDE;

        private:
            MainFrame* m_frame;
        };
    }
}
