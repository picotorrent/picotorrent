#include "ViewPreferencesController.hpp"

#include <libtorrent/session.hpp>

#include "../Core/SessionSettings.hpp"
#include "../PropertySheets/Preferences/PreferencesSheet.hpp"
#include "../resources.h"

namespace lt = libtorrent;
using Controllers::ViewPreferencesController;

ViewPreferencesController::ViewPreferencesController(const std::shared_ptr<lt::session>& session)
    : m_session(session)
{
}

void ViewPreferencesController::Execute()
{
    PropertySheets::Preferences::PreferencesSheet sheet;
    sheet.DoModal();

    lt::settings_pack settings = Core::SessionSettings::Get();
    m_session->apply_settings(settings);
}
