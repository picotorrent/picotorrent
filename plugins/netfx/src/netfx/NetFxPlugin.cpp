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

void NetFxPlugin::load()
{
    bridge_->Load();
}

void NetFxPlugin::unload()
{
    bridge_->Unload();
}
