using PicoTorrent.UI;
using System;

namespace PicoTorrent.Plugins.Pushbullet
{
    public class PushbulletPlugin : IPlugin
    {
        public string Name => "Pushbullet";

        public Version Version => typeof(PushbulletPlugin).Assembly.GetName().Version;

        public IWindow GetWindow()
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
