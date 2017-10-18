#include "taskbaricon.hpp"

using pt::TaskBarIcon;

TaskBarIcon::TaskBarIcon(wxFrame* parent)
	: m_parent(parent)
{
}

wxMenu* TaskBarIcon::CreatePopupMenu()
{
	wxMenu* menu = new wxMenu();
	menu->Append(wxID_ANY, "Add torrents");
	menu->Append(wxID_ANY, "Add magnet links");
	menu->AppendSeparator();
	menu->Append(wxID_ANY, "Exit");

	return menu;
}
