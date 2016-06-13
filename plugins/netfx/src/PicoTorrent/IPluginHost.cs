using PicoTorrent.Core;
using PicoTorrent.Logging;
using PicoTorrent.UI;

namespace PicoTorrent
{
    public interface IPluginHost
    {
        ILogger Logger { get; }

        IMainWindow MainWindow { get; }

        ISession Session { get; }

        IVersionInformation VersionInformation { get; }
    }
}
