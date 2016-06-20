using PicoTorrent;
using PicoTorrent.Plugins.Pushbullet;

[assembly: PluginFactory(typeof(Factory))]

namespace PicoTorrent.Plugins.Pushbullet
{
    public class Factory
    {
        public static void Register(IContainer container)
        {
            // Register plugin
            container.Register<IPlugin, PushbulletPlugin>();
        }
    }
}
