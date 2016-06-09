#include <picotorrent/netfx/managed_bridge.hpp>

#include <picotorrent/core/session.hpp>
#include <picotorrent/netfx/clr/plugin_host.hpp>
#include <picotorrent/netfx/clr/session.hpp>

using picotorrent::core::session;
using picotorrent::netfx::managed_bridge;

managed_bridge::managed_bridge(session *sess)
{
    host_ = gcnew PicoTorrent::PluginHost(
        gcnew PicoTorrent::Core::Session(sess));
}

void managed_bridge::load()
{
    host_->Load();
}

void managed_bridge::unload()
{
    host_->Unload();
}
