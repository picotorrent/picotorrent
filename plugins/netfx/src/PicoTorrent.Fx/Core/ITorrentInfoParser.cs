namespace PicoTorrent.Core
{
    public interface ITorrentInfoParser
    {
        ITorrentInfo Parse(string fileName);
    }
}
