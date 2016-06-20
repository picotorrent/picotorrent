using PicoTorrent;
using PicoTorrent.Plugins.Pushbullet;
using PicoTorrent.Plugins.Pushbullet.UI;
using PicoTorrent.UI;

[assembly: PluginFactory(typeof(Factory))]

namespace PicoTorrent.Plugins.Pushbullet
{
    public class Factory
    {
        public static void Register(IContainer container)
        {
            container.Register<IConfigurationWindow, PushbulletConfig>();

            // Register plugin
            container.Register<IPlugin, PushbulletPlugin>();
        }
    }
}
