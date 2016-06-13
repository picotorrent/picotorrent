namespace PicoTorrent
{
    public interface IVersionInformation
    {
        string Branch { get; }

        string Commitish { get; }

        string CurrentVersion { get; }
    }
}
