#include <picotorrent/api.hpp>

#include "Controllers/CheckForUpdateController.hpp"

Controllers::CheckForUpdateController* g_controller;

class MenuItem : public IMenuItem
{
public:
    std::string GetText()
    {
        return "Check for updates";
    }

    void OnClick()
    {
        g_controller->Execute();
    }
};


extern "C" bool __declspec(dllexport) pico_init_plugin(int version, IPicoTorrent* pico)
{
    if (version != PICOTORRENT_API_VERSION)
    {
        return false;
    }

    g_controller = new Controllers::CheckForUpdateController(pico);
    g_controller->Execute();

    pico->AddMenuItem(new MenuItem());

    return true;
}
