#include "ImportTorrentsController.hpp"

#include <picotorrent/api.hpp>

#include "../resources.h"
#include "../Wizard/ImportWizard.hpp"

using Controllers::ImportTorrentsController;

ImportTorrentsController::ImportTorrentsController(std::shared_ptr<IPicoTorrent> pico)
    : m_pico(pico)
{
}

void ImportTorrentsController::Execute()
{
    Wizard::ImportWizard wiz(m_pico);
    wiz.DoModal();
}
