#include "server.hpp"

#include "../mainframe.hpp"
#include "applicationoptionsconnection.hpp"

using pt::ipc::Server;

Server::Server(pt::MainFrame* mainFrame)
    : m_frame(mainFrame)
{
}

wxConnectionBase* Server::OnAcceptConnection(const wxString& topic)
{
    if (topic == "ApplicationOptions")
    {
        return new ApplicationOptionsConnection(m_frame);
    }

    return nullptr;
}
