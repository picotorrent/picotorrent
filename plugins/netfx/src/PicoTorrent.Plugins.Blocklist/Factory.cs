using PicoTorrent;
using PicoTorrent.Plugins.Blocklist;
using PicoTorrent.Plugins.Blocklist.UI;
using PicoTorrent.UI;

[assembly: PluginFactory(typeof(Factory))]

namespace PicoTorrent.Plugins.Blocklist
{
    public static class Factory
    {
        public static void Register(IContainer container)
        {
            container.Register<IConfigControl, ConfigControl>();
            container.Register<IConfigurationWindow, BlocklistConfigurationWindow>();

            container.Register<IPlugin, BlocklistPlugin>();
        }
    }
}
