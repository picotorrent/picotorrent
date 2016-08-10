#pragma once
#pragma managed

#include <vcclr.h>
#include <windows.h>

#include <picotorrent/plugin.hpp>

namespace picotorrent { namespace extensibility { class plugin_host; } }

namespace PicoTorrent
{
    interface class IPluginEngine;
}

class ClrBridge
{
public:
    ClrBridge(picotorrent::extensibility::plugin_host*);

    std::vector<picotorrent::plugin_ptr> GetPlugins();

private:
    gcroot<PicoTorrent::IPluginEngine^> _engine;
};
