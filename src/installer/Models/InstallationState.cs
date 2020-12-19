namespace PicoTorrentBootstrapper.Models
{
    /// <summary>
    /// The states of installation.
    /// </summary>
    public enum InstallationState
    {
        Initializing,
        Detecting,
        Waiting,
        Planning,
        Applying,
        Applied,
        Failed,
    }
}
