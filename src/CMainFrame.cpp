#include "CMainFrame.hpp"

#include <libtorrent/session.hpp>

namespace lt = libtorrent;

void CMainFrame::OnAlertNotify()
{
    PostMessage(LT_SESSION_ALERT);
}

LRESULT CMainFrame::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    // Create the UI

    // Disable session interaction while session loading

    // Create session
    lt::settings_pack settings;
    settings.set_int(lt::settings_pack::alert_mask, lt::alert::category_t::all_categories);

    m_session = std::make_shared<lt::session>(settings);
    m_session->set_alert_notify(std::bind(&CMainFrame::OnAlertNotify, this));

    return 0;
}

LRESULT CMainFrame::OnSessionAlert(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return 0;
}
