using System;

namespace PicoTorrent.Plugins.Blocklist.UI
{
    public interface IConfigControl
    {
        string BlocklistUrl { get; set; }

        event EventHandler BlocklistUrlChanged;

        bool EnableBlocklist { get; set; }

        event EventHandler EnableBlocklistChanged;

        IntPtr Handle { get; }
    }
}
