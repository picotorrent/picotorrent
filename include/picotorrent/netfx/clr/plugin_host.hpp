#pragma once

namespace PicoTorrent
{
namespace Core
{
    ref class Session;
}

    ref class PluginHost
    {
    public:
        PluginHost(Core::Session^);

        void Load();
        void Unload();

    private:
        Core::Session^ _session;
    };
}
