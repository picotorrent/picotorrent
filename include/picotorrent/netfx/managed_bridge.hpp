#pragma once
#pragma managed

#include <vcclr.h>

namespace PicoTorrent
{
    ref class PluginHost;
}

namespace picotorrent
{
namespace core
{
    class session;
}
namespace netfx
{
    class managed_bridge
    {
    public:
        managed_bridge(core::session*);

        void load();
        void unload();

    private:
        gcroot<PicoTorrent::PluginHost^> host_;
    };
}
}
