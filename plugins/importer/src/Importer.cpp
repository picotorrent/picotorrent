#include <picotorrent/api.hpp>

#include "Controllers/ImportTorrentsController.hpp"

using Controllers::ImportTorrentsController;

extern "C" bool __declspec(dllexport) pico_init_plugin(int version, std::shared_ptr<IPicoTorrent> pico)
{
    if (version != PICOTORRENT_API_VERSION)
    {
        return false;
    }

    MenuItem item{ L"Import torrents" };
    item.onClick = [pico]()
    {
        ImportTorrentsController import(pico);
        import.Execute();
    };

    pico->AddMenuItem(item);

    return true;
}
