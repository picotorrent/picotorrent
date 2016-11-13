#include <picotorrent/api.hpp>

#include "Engine/JavaScriptEngine.hpp"

Engine::JavaScriptEngine* g_jsEngine;

extern "C" bool __declspec(dllexport) pico_init_plugin(int version, std::shared_ptr<IPicoTorrent> pico)
{
    if (version != PICOTORRENT_API_VERSION)
    {
        return false;
    }

    g_jsEngine = new Engine::JavaScriptEngine(pico);
    g_jsEngine->Load();

    return true;
}
