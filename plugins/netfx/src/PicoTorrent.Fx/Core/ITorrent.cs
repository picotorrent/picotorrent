namespace PicoTorrent.Core
{
    public interface ITorrent
    {
        string Name { get; }

        void Pause();

        void Resume(bool force = false);
    }
}
