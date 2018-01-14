#include "taskbaricon.hpp"

#include <wx/taskbarbutton.h>

#include "addtorrentproc.hpp"
#include "config.hpp"
#include "environment.hpp"
#include "preferencesdlg.hpp"
#include "sessionstate.hpp"
#include "translator.hpp"

using pt::TaskBarIcon;

wxBEGIN_EVENT_TABLE(TaskBarIcon, wxTaskBarIcon)
    EVT_MENU(ptID_ADD_TORRENT, TaskBarIcon::OnAddTorrent)
    EVT_MENU(ptID_ADD_MAGNET_LINK, TaskBarIcon::OnAddMagnetLink)
    EVT_MENU(ptID_PREFERENCES, TaskBarIcon::OnViewPreferences)
    EVT_MENU(wxID_EXIT, TaskBarIcon::OnExit)
    EVT_TASKBAR_LEFT_DCLICK(TaskBarIcon::OnLeftButtonDClick)
wxEND_EVENT_TABLE()

TaskBarIcon::TaskBarIcon(wxFrame* parent,
    std::shared_ptr<pt::Configuration> cfg,
    std::shared_ptr<pt::Environment> env,
    std::shared_ptr<pt::Translator> translator,
    std::shared_ptr<pt::SessionState> state)
    : m_parent(parent),
    m_cfg(cfg),
    m_env(env),
    m_trans(translator),
    m_state(state)
{
}

void TaskBarIcon::SetPicoIcon()
{
    SetIcon(wxICON(AppIcon), "PicoTorrent");
}

wxMenu* TaskBarIcon::CreatePopupMenu()
{
    wxMenu* menu = new wxMenu();
    menu->Append(ptID_ADD_TORRENT, i18n(m_trans, "amp_add_torrent"));
    menu->Append(ptID_ADD_MAGNET_LINK, i18n(m_trans, "amp_add_magnet_link_s"));
    menu->AppendSeparator();
    menu->Append(ptID_PREFERENCES, i18n(m_trans, "preferences"));
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, i18n(m_trans, "amp_exit"));

    return menu;
}

void TaskBarIcon::OnAddTorrent(wxCommandEvent& WXUNUSED(event))
{
    AddTorrentProcedure proc(m_parent, m_cfg, m_trans, m_state);
    proc.Execute();
}

void TaskBarIcon::OnAddMagnetLink(wxCommandEvent& WXUNUSED(event))
{
    AddTorrentProcedure proc(m_parent, m_cfg, m_trans, m_state);
    proc.ExecuteMagnet();
}

void TaskBarIcon::OnExit(wxCommandEvent& WXUNUSED(event))
{
    m_parent->Close(true);
}

void TaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent& WXUNUSED(event))
{
    m_parent->MSWGetTaskBarButton()->Show();
    m_parent->Restore();
    m_parent->Raise();
    m_parent->Show();
}

void TaskBarIcon::OnViewPreferences(wxCommandEvent& WXUNUSED(event))
{
    PreferencesDialog dlg(
        m_parent,
        m_env,
        m_cfg,
        m_state,
        shared_from_this(),
        m_trans);

    dlg.ShowModal();
}
