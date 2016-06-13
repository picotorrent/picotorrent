#pragma once
#pragma managed

#include <vcclr.h>
#include <windows.h>

namespace picotorrent { namespace core { class session; } }

namespace PicoTorrent
{
    interface class IPluginEngine;
}

class ClrBridge
{
public:
    ClrBridge(picotorrent::core::session*, HWND hWnd);
    ~ClrBridge();

    void Load();
    void Unload();

private:
    gcroot<PicoTorrent::IPluginEngine^> _engine;
};
