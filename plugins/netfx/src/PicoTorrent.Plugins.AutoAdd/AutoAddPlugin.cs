using PicoTorrent.UI;
using System;

namespace PicoTorrent.Plugins.AutoAdd
{
    public class AutoAddPlugin : IPlugin
    {
        public string Name => "AutoAdd";

        public Version Version => typeof(AutoAddPlugin).Assembly.GetName().Version;

        public IConfigurationWindow GetConfigurationWindow()
        {
            return null;
        }

        public void Load()
        {
        }

        public void Unload()
        {
        }
    }
}
