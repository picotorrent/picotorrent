#include "ViewPreferencesController.hpp"

#include <libtorrent/session.hpp>

#include "../Configuration.hpp"
#include "../Core/SessionSettings.hpp"
#include "../PropertySheets/Preferences/PreferencesSheet.hpp"
#include "../resources.h"
#include "../UI/NotifyIcon.hpp"

namespace lt = libtorrent;
using Controllers::ViewPreferencesController;

ViewPreferencesController::ViewPreferencesController(
    const std::shared_ptr<UI::NotifyIcon>& notifyIcon,
    const std::shared_ptr<lt::session>& session)
    : m_notifyIcon(notifyIcon),
    m_session(session)
{
}

void ViewPreferencesController::Execute()
{
    PropertySheets::Preferences::PreferencesSheet sheet(m_notifyIcon);
    sheet.DoModal();

    // Save configuration to disk
    Configuration& cfg = Configuration::GetInstance();
    cfg.Save();

    lt::settings_pack settings = Core::SessionSettings::Get();
    m_session->apply_settings(settings);
}
