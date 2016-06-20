using PicoTorrent.Plugins.AutoAdd.UI;
using PicoTorrent.UI;
using System;

namespace PicoTorrent.Plugins.AutoAdd
{
    public class AutoAddPlugin : IPlugin
    {
        public string Name => "AutoAdd";

        public Version Version => typeof(AutoAddPlugin).Assembly.GetName().Version;

        public IWindow GetWindow()
        {
            return new ConfigWindow();
        }

        public void Load()
        {
        }

        public void Unload()
        {
        }
    }
}
