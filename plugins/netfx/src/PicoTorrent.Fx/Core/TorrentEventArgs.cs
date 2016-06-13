using System;

namespace PicoTorrent.Core
{
    public sealed class TorrentEventArgs : EventArgs
    {
        public TorrentEventArgs(ITorrent torrent)
        {
            Torrent = torrent;
        }

        public ITorrent Torrent { get; }
    }
}
