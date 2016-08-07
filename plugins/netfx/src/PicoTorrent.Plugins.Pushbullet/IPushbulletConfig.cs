using System;
using System.ComponentModel;

namespace PicoTorrent.Plugins.Pushbullet
{
    [Flags]
    public enum Event
    {
        None = 0x0,
        [Description("Torrent added")]
        TorrentAdded = 0x1,
        [Description("Torrent finished")]
        TorrentFinished = 0x2,
        [Description("Torrent removed")]
        TorrentRemoved = 0x4
    }

    public interface IPushbulletConfig
    {
        string AccessToken { get; set; }

        Event Events { get; set; }
    }
}
