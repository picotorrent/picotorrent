#include <picotorrent/netfx/clr_plugin.hpp>

#include <picotorrent/core/session.hpp>
#include <picotorrent/netfx/managed_bridge.hpp>

using picotorrent::core::session;
using picotorrent::netfx::managed_bridge;
using picotorrent::netfx::clr_plugin;

clr_plugin::clr_plugin(session* sess)
{
    bridge_ = new managed_bridge(sess);
}

clr_plugin::~clr_plugin()
{
    delete bridge_;
}

void clr_plugin::load()
{
    bridge_->load();
}

void clr_plugin::unload()
{
    bridge_->unload();
}
