#pragma once
#pragma unmanaged

#include <picotorrent/plugin.hpp>
#include <windows.h>

namespace picotorrent { namespace extensibility { class plugin_host; } }

class ClrBridge;

class NetFxPlugin : public picotorrent::plugin
{
public:
    NetFxPlugin(picotorrent::extensibility::plugin_host*);
    ~NetFxPlugin();

    void load();
    void unload();

private:
    ClrBridge* bridge_;
};

extern "C"
{
    __declspec(dllexport) picotorrent::plugin* create_picotorrent_plugin(picotorrent::extensibility::plugin_host* host)
    {
        return new NetFxPlugin(host);
    }
}
