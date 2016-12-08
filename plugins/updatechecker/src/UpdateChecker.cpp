#include <picotorrent/api.hpp>

#include "Controllers/CheckForUpdateController.hpp"

Controllers::CheckForUpdateController* g_controller;

extern "C" bool __declspec(dllexport) pico_init_plugin(int version, std::shared_ptr<IPicoTorrent> pico)
{
    if (version != PICOTORRENT_API_VERSION)
    {
        return false;
    }

    g_controller = new Controllers::CheckForUpdateController(pico);
    g_controller->Execute();

    MenuItem mi;
    mi.onClick = []() { g_controller->Execute(true); };
    mi.text = pico->GetTranslator()->Translate("amp_check_for_update");

    pico->AddMenuItem(mi);

    return true;
}
