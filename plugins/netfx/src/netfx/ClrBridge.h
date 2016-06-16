#pragma once
#pragma managed

#include <vcclr.h>
#include <windows.h>

namespace picotorrent { namespace extensibility { class plugin_host; } }

namespace PicoTorrent
{
    interface class IPluginEngine;
}

class ClrBridge
{
public:
    ClrBridge(picotorrent::extensibility::plugin_host*);
    ~ClrBridge();

    void Load();
    void Unload();

private:
    gcroot<PicoTorrent::IPluginEngine^> _engine;
};
