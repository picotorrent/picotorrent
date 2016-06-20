using System;
using PicoTorrent.UI;

namespace PicoTorrent.Plugins.Pushbullet.UI
{
    public sealed class PushbulletConfig : IConfigurationWindow
    {
        private readonly ConfigControl _control;

        public PushbulletConfig()
        {
            _control = new ConfigControl();
        }

        public IntPtr GetHandle()
        {
            return _control.Handle;
        }

        public void Load()
        {
            throw new NotImplementedException();
        }

        public void Save()
        {
            throw new NotImplementedException();
        }
    }
}
