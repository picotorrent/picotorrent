#include <picotorrent/netfx/clr/plugin_host.hpp>

#include <picotorrent/netfx/clr/session.hpp>

using PicoTorrent::PluginHost;
using PicoTorrent::Core::Session;

PluginHost::PluginHost(Session^ session)
{
    _session = session;
}

void PluginHost::Load()
{
    System::Windows::Forms::MessageBox::Show("Hello!");
}

void PluginHost::Unload()
{
}
