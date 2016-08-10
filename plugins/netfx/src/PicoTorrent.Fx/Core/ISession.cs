using System;

namespace PicoTorrent.Core
{
    public interface ISession
    {
        event EventHandler<TorrentEventArgs> TorrentAdded;
        event EventHandler<TorrentEventArgs> TorrentFinished;
        event EventHandler<TorrentEventArgs> TorrentRemoved;

        void AddTorrent(string torrentFile, string savePath = "");
    }
}
