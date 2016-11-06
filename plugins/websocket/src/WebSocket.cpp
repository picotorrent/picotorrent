#include <picotorrent/api.hpp>

#include "TorrentEventSink.hpp"

extern "C" bool __declspec(dllexport) pico_init_plugin(int version, std::shared_ptr<IPicoTorrent> pico)
{
    if (version != PICOTORRENT_API_VERSION)
    {
        return false;
    }

    pico->RegisterEventSink(std::make_shared<TorrentEventSink>());

    return true;
}
