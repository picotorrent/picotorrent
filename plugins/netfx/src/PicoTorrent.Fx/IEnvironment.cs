namespace PicoTorrent
{
    public interface IEnvironment
    {
        string ApplicationPath { get; }

        string DataPath { get; }

        bool IsInstalled { get; }
    }
}
