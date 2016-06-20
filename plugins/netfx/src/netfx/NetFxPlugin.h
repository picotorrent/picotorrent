#pragma once
#pragma unmanaged

#include <picotorrent/plugin.hpp>
#include <vector>

namespace picotorrent { namespace extensibility { class plugin_host; } }

class ClrBridge;

class NetFxPlugin
{
public:
    NetFxPlugin(picotorrent::extensibility::plugin_host*);
    ~NetFxPlugin();

    std::vector<picotorrent::plugin_ptr> get_plugins();

private:
    ClrBridge* bridge_;
};

extern "C"
{
    __declspec(dllexport) picotorrent::plugin_wrapper* create_picotorrent_plugin(picotorrent::extensibility::plugin_host* host)
    {
        NetFxPlugin nfp(host);
        return new picotorrent::plugin_wrapper{ nfp.get_plugins() };
    }
}
