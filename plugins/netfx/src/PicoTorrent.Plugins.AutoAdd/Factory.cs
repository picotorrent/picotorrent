using PicoTorrent;
using PicoTorrent.Plugins.AutoAdd;
using PicoTorrent.Plugins.AutoAdd.Data;
using PicoTorrent.Plugins.AutoAdd.UI;
using PicoTorrent.UI;

[assembly: PluginFactory(typeof(Factory))]

namespace PicoTorrent.Plugins.AutoAdd
{
    public class Factory
    {
        public static void Register(IContainer container)
        {
            container.Register<IAutoAddConfig, AutoAddConfig>();
            container.Register<IConfigControl, ConfigControl>();
            container.Register<IConfigurationWindow, AutoAddConfigurationWindow>();
            container.Register<IFolderWatcher, FolderWatcher>();
            container.RegisterSingleton<IHistoryRepository, HistoryRepository>();

            // Register plugin
            container.Register<IPlugin, AutoAddPlugin>();
        }
    }
}
