using System;
using PicoTorrent.UI;

namespace PicoTorrent
{
    public abstract class Plugin : IPlugin
    {
        public string Name => GetType().Name.TrimEnd('P', 'l', 'u', 'g', 'i', 'n');

        public Version Version => GetType().Assembly.GetName().Version;

        public virtual IConfigurationWindow GetConfigurationWindow()
        {
            return null;
        }

        public abstract void Load();

        public virtual void Unload()
        {
        }
    }
}
