#include "NetFxPlugin.h"

#include "ClrBridge.h"

NetFxPlugin::NetFxPlugin(picotorrent::core::session *sess, HWND hWnd)
{
    bridge_ = new ClrBridge(sess, hWnd);
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
