#include "UpdateAvailableDialog.hpp"

#include <picotorrent/api.hpp>

using Dialogs::UpdateAvailableDialog;

UpdateAvailableDialog::UpdateAvailableDialog(std::shared_ptr<IPicoTorrent> pico)
    : m_pico(pico)
{
}

void UpdateAvailableDialog::Show()
{
}
