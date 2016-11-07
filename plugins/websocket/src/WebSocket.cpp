#include <picotorrent/api.hpp>

#include "Config/WebSocketConfig.hpp"
#include "TorrentEventSink.hpp"

extern "C" bool __declspec(dllexport) pico_init_plugin(int version, std::shared_ptr<IPicoTorrent> pico)
{
    if (version != PICOTORRENT_API_VERSION)
    {
        return false;
    }

    Config::WebSocketConfig config(pico->GetConfiguration());

    if (config.IsEnabled())
    {
        pico->RegisterEventSink(std::make_shared<TorrentEventSink>(config.ListenPort()));
    }

    return true;
}
