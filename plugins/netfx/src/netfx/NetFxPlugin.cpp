#include "NetFxPlugin.h"

#include "ClrBridge.h"

NetFxPlugin::NetFxPlugin(picotorrent::extensibility::plugin_host* host)
{
    bridge_ = new ClrBridge(host);
}

NetFxPlugin::~NetFxPlugin()
{
    delete bridge_;
}

std::vector<picotorrent::plugin_ptr> NetFxPlugin::get_plugins()
{
    return bridge_->GetPlugins();
}
