using PicoTorrent;
using PicoTorrent.Plugins.UpdateChecker;
using PicoTorrent.Plugins.UpdateChecker.Dialogs;

[assembly: PluginFactory(typeof(Factory))]

namespace PicoTorrent.Plugins.UpdateChecker
{
    public class Factory
    {
        public static void Register(IContainer container)
        {
            container.Register<IUpdateCheckerConfig, UpdateCheckerConfig>();
            container.Register<INoUpdateAvailableDialog, NoUpdateAvailableDialog>();
            container.Register<IUpdateAvailableDialog, UpdateAvailableDialog>();

            // Register plugin
            container.Register<IPlugin, UpdateCheckerPlugin>();
        }
    }
}
