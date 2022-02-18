#include "taskbaricon.hpp"

#include "ids.hpp"
#include "translator.hpp"

using pt::UI::TaskBarIcon;

TaskBarIcon::TaskBarIcon(wxWindow* parent)
    : m_parent(parent)//,
    //m_icon(wxIcon(wxICON(AppIcon)))
{
}

void TaskBarIcon::Hide()
{
    RemoveIcon();
}

void TaskBarIcon::Show()
{
    SetIcon(m_icon, "PicoTorrent");
}

wxMenu* TaskBarIcon::CreatePopupMenu()
{
    wxMenu* menu = new wxMenu();
    menu->Append(ptID_EVT_ADD_TORRENT, i18n("amp_add_torrent"));
    menu->Append(ptID_EVT_ADD_MAGNET_LINK, i18n("amp_add_magnet_link_s"));
    menu->AppendSeparator();
    menu->Append(ptID_EVT_VIEW_PREFERENCES, i18n("preferences"));
    menu->AppendSeparator();
    menu->Append(ptID_EVT_EXIT, i18n("amp_exit"));
    return menu;
}
