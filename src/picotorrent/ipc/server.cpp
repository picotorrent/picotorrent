#include "server.hpp"

#include "applicationoptionsconnection.hpp"
#include "../ui/mainframe.hpp"

using pt::IPC::Server;

Server::Server(pt::UI::MainFrame* mainFrame)
    : m_frame(mainFrame)
{
    Create("PicoTorrent");
}

Server::~Server()
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
