using System;

namespace PicoTorrent.Core
{
    public interface ISession
    {
        event EventHandler<TorrentEventArgs> TorrentAdded;
    }
}
