using PicoTorrent;
using PicoTorrent.Plugins.AutoAdd;

[assembly: PluginFactory(typeof(Factory))]

namespace PicoTorrent.Plugins.AutoAdd
{
    public class Factory
    {
        public static void Register(IContainer container)
        {
            // Register plugin
            container.Register<IPlugin, AutoAddPlugin>();
        }
    }
}
