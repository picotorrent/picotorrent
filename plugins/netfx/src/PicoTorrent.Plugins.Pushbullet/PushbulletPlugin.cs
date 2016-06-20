using PicoTorrent.UI;
using System;

namespace PicoTorrent.Plugins.Pushbullet
{
    public class PushbulletPlugin : IPlugin
    {
        private readonly IConfigurationWindow _configWindow;

        public PushbulletPlugin(IConfigurationWindow configWindow)
        {
            if (configWindow == null) throw new ArgumentNullException(nameof(configWindow));
            _configWindow = configWindow;
        }

        public string Name => "Pushbullet";

        public Version Version => typeof(PushbulletPlugin).Assembly.GetName().Version;

        public IConfigurationWindow GetConfigurationWindow()
        {
            return _configWindow;
        }

        public void Load()
        {
        }

        public void Unload()
        {
        }
    }
}
