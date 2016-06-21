using System;

namespace PicoTorrent.Plugins.Pushbullet.UI
{
    public interface IConfigControl
    {
        event EventHandler AccessTokenChanged;

        event EventHandler TestAccessToken;

        string AccessToken { get; set; }

        IntPtr Handle { get; }

        void Disable();
        void Enable();
    }
}
