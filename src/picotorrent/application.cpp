#include "application.hpp"
#include "mainframe.hpp"

using pt::Application;

Application::Application()
	: m_mainFrame(new MainFrame())
{
}

bool Application::OnInit()
{
	m_mainFrame->Show(true);
    return true;
}
